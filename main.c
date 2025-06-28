#include <stdlib.h>
#include "quickjs.h"
#include "minnet.h"
#include "raylib.h"
#include "engine.h"

#include "./lib/clay/clay.h"
#include "./lib/clay/renderers/raylib/clay_renderer_raylib.c"
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

    const Clay_Color COLOR_LIGHT = (Clay_Color){224, 215, 210, 255};
    const Clay_Color COLOR_RED = (Clay_Color){168, 66, 28, 255};
    const Clay_Color COLOR_ORANGE = (Clay_Color){225, 138, 50, 255};

    Font fonts[1];
    fonts[0] = LoadFontEx("./lib/clay/examples/raylib-multi-context/resources/Roboto-Regular.ttf", 48, 0, 400);
    SetTextureFilter(fonts[0].texture, TEXTURE_FILTER_BILINEAR);
    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

    while (!WindowShouldClose())
    {
        Clay_SetLayoutDimensions((Clay_Dimensions){GetScreenWidth(), GetScreenHeight()});

        Clay_BeginLayout();
        // An example of laying out a UI with a fixed width sidebar and flexible width main content
        CLAY({.id = CLAY_ID("OuterContainer"),
              .layout = {.layoutDirection = CLAY_LEFT_TO_RIGHT, // Add this for side-by-side layout
                         .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)},
                         .padding = CLAY_PADDING_ALL(16)},
              .backgroundColor = {250, 250, 255, 255}})
        {
            CLAY({.id = CLAY_ID("SideBar"),
                  .layout = {.layoutDirection = CLAY_TOP_TO_BOTTOM,
                             .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)}}, // Fixed width, full height
                  .backgroundColor = COLOR_LIGHT})
            {
                // Sidebar content goes here
            }

            CLAY({.id = CLAY_ID("MainContent"),
                  .layout = {.sizing = {CLAY_SIZING_GROW(1), CLAY_SIZING_GROW(1)}}, // Grow to fill remaining space
                  .backgroundColor = COLOR_RED})
            {
                CLAY_TEXT(
                    CLAY_STRING("This is the main content"),
                    CLAY_TEXT_CONFIG({
                        .fontSize = 20,
                        .textColor = {0, 0, 0, 255}, // Changed alpha to 255 for visible text
                    }));
            }
        }

        Clay_RenderCommandArray renderCommands = Clay_EndLayout();
        BeginDrawing();
        ClearBackground(BLACK);
        Clay_Raylib_Render(renderCommands, (Font[]){});
        EndDrawing();
    }

    engine_deinit(ctx);
    gui_deinit();

    return 0;
}