#include <stdlib.h>
#include "../lib/quickjs/quickjs.h"
#include "../lib/minnet-quickjs/minnet.h"
#include "../lib/raylib/src/raylib.h"
#include "engine.h"
#include "terminal_colors.h"

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
    if (engine_exception_handled(ctx))
    {
        JS_FreeValue(ctx, ret);
        exit(3);
    }
    JS_FreeValue(ctx, ret);
}

int main()
{
    engine_init(&rt, &ctx);

    int windowWidth = 600;
    int windowHeight = 300;
    Font *fonts = gui_init(windowWidth, windowHeight);

    char *rawJS = LoadFileText(scriptName);
    JSValue ret = JS_Eval(ctx, rawJS, strlen(rawJS), scriptName, JS_EVAL_TYPE_MODULE);

    void *watcher = wtr_watcher_open(".", inputWatcher, ctx);

    float scaleX = (float)GetRenderWidth() / (float)GetScreenWidth();
    float scaleY = (float)GetRenderHeight() / (float)GetScreenHeight();
    while (!WindowShouldClose())
    {
        Clay_SetLayoutDimensions((Clay_Dimensions){GetScreenWidth() * scaleX, GetScreenHeight() * scaleY});

        Clay_RenderCommandArray renderCommands = gui_create_render_tree();

        BeginDrawing();
        ClearBackground(BLACK);
        Clay_Raylib_Render(renderCommands, fonts);
        EndDrawing();

        if (engine_exception_handled(ctx))
        {
            JSValue exception = JS_GetException(ctx);
            char *exStr = JS_ToCString(ctx, exception);
            fprintf(stderr, "JS Exception: %s\n", exStr);
            break;
        }
    }

    engine_deinit(ctx);
    gui_deinit(fonts);

    return 0;
}