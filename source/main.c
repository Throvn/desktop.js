#include <stdlib.h>
#include "../lib/quickjs/quickjs.h"
#include "../lib/minnet-quickjs/minnet.h"
#include "../lib/raylib/src/raylib.h"
#include "engine.h"

#include "../lib/clay/clay.h"
#include "../lib/clay/renderers/raylib/clay_renderer_raylib.c"
#include "gui.h"

int main()
{
    JSRuntime *rt;
    JSContext *ctx;
    engine_init(&rt, &ctx);

    int windowWidth = 600;
    int windowHeight = 300;
    gui_init(windowWidth, windowHeight);

    char *scriptName = "./test.js";
    char *rawJS = LoadFileText(scriptName);
    JSValue ret = JS_Eval(ctx, rawJS, strlen(rawJS), scriptName, JS_EVAL_TYPE_MODULE);

    if (JS_IsException(ret))
    {
        JSValue exception = JS_GetException(ctx);
        const char *exceptionString = JS_ToCString(ctx, exception);
        fprintf(stderr, "%s\n", exceptionString);

        JSValue stack_val = JS_GetPropertyStr(ctx, exception, "stack");
        if (!JS_IsUndefined(stack_val))
        {
            JSValue stack_str_val = JS_ToString(ctx, stack_val);
            if (!JS_IsException(stack_str_val))
            {
                const char *stack_str = JS_ToCString(ctx, stack_str_val);
                if (stack_str)
                {
                    fprintf(stderr, "JavaScript Error Stack:\n%s\n", stack_str);
                    JS_FreeCString(ctx, stack_str);
                }
            }
            JS_FreeValue(ctx, stack_str_val);
        }
        JS_FreeValue(ctx, stack_val);

        JS_FreeValue(ctx, exception);
    }

    Font fonts[1];
    fonts[0] = LoadFontEx("./lib/clay/examples/raylib-multi-context/resources/Roboto-Regular.ttf", 200, 0, 400);
    SetTextureFilter(fonts[0].texture, TEXTURE_FILTER_BILINEAR);
    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

    while (!WindowShouldClose())
    {
        Clay_SetLayoutDimensions((Clay_Dimensions){GetScreenWidth(), GetScreenHeight()});

        Clay_RenderCommandArray renderCommands = gui_create_render_tree();

        BeginDrawing();
        ClearBackground(BLACK);
        Clay_Raylib_Render(renderCommands, fonts);
        EndDrawing();

        if (1 == JS_HasException(ctx))
        {
            JSValue exception = JS_GetException(ctx);
            char *exStr = JS_ToCString(ctx, exception);
            fprintf(stderr, "JS Exception: %s\n", exStr);
            break;
        }
    }

    engine_deinit(ctx);
    gui_deinit();

    return 0;
}