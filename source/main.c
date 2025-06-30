#include <stdlib.h>
#include "../lib/quickjs/quickjs.h"
#include "../lib/minnet-quickjs/minnet.h"
#include "../lib/raylib/src/raylib.h"
#include "engine.h"

#include "../lib/clay/clay.h"
#include "../lib/clay/renderers/raylib/clay_renderer_raylib.c"
#include "gui.h"
#include "../lib/watcher/watcher-c.h"

char *scriptName = "js/test.js";
JSRuntime *rt;
JSContext *ctx;

void inputWatcher(struct wtr_watcher_event event, void *ctx)
{
    printf("Watcher event: %d %s\n", event.effect_type, event.path_name);
    char *rawJS = LoadFileText(scriptName);
    JSValue ret = JS_Eval(ctx, rawJS, strlen(rawJS), scriptName, JS_EVAL_TYPE_MODULE);
    if (JS_IsException(ret))
    {
        printf("FUCK\n");
        exit(3);
    }
}

int main()
{
    engine_init(&rt, &ctx);

    int windowWidth = 600;
    int windowHeight = 300;
    gui_init(windowWidth, windowHeight);

    char *rawJS = LoadFileText(scriptName);
    JSValue ret = JS_Eval(ctx, rawJS, strlen(rawJS), scriptName, JS_EVAL_TYPE_MODULE);

    void *watcher = wtr_watcher_open(".", inputWatcher, ctx);

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