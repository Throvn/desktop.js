#include <stdlib.h>
#include "../lib/quickjs/quickjs.h"
#include "../lib/minnet-quickjs/minnet.h"
#include "../lib/raylib/src/raylib.h"
#include "engine.h"
#include "terminal_colors.h"

#include "../lib/clay/clay.h"
#include "gui.h"

#define DMON_IMPL
#include "../lib/dmon/dmon.h"

#if DMON_OS_LINUX
#include "../lib/dmon/dmon_extra.h"
#endif

extern void Clay_Raylib_Render(Clay_RenderCommandArray renderCommands, Font *fonts);
JSModuleDef *js_init_module_gui(JSContext *ctx, const char *module_name);

JSRuntime *rt;
JSContext *ctx;

static void watch_callback(dmon_watch_id watch_id, dmon_action action, const char *rootdir,
                           const char *filepath, const char *oldfilepath, void *scriptPath)
{
    (void)(watch_id);

    switch (action)
    {
    case DMON_ACTION_CREATE:
        printf("CREATE: [%s]%s\n", rootdir, filepath);
        break;
    case DMON_ACTION_DELETE:
        printf("DELETE: [%s]%s\n", rootdir, filepath);
        break;
    case DMON_ACTION_MODIFY:
        printf("MODIFY: [%s]%s\n", rootdir, filepath);
        break;
    case DMON_ACTION_MOVE:
        printf("MOVE: [%s]%s -> [%s]%s\n", rootdir, oldfilepath, rootdir, filepath);
        break;
    }

    char *rawJS = LoadFileText(scriptPath);
    JSValue ret = JS_Eval(ctx, rawJS, strlen(rawJS), scriptPath, JS_EVAL_TYPE_MODULE);
    free(rawJS);
    if (engine_exception_handled(ctx))
    {
        JS_FreeValue(ctx, ret);
        exit(3);
    }
    JS_FreeValue(ctx, ret);
}

typedef struct
{
    char *program;
    int isRun;
    int isWatch;
    int isHelp;
    char *path;
} CliArgs;

CliArgs args_init(int argc, char *argv[])
{
    CliArgs args = {0};
    if (argc <= 0)
        return args;

    args.program = argv[0];
    if (argc > 1)
    {
        args.isRun = 0 == strcmp(argv[1], "run");
        args.isWatch = 0 == strcmp(argv[1], "watch");
        args.isHelp = 0 == strcmp(argv[1], "help");
    }
    if (argc > 2)
    {
        char *path = realpath(argv[2], NULL);
        args.path = path;
    }
    return args;
}

int main(int argc, char *argv[])
{
    printf("djs v0.1.0\n");
    CliArgs args = args_init(argc, argv);

    if (args.path == NULL)
    {
        printf("Note: djs run <path> | Runs in prod. mode\n");
        printf("Note: djs watch <path> | Runs live reload mode\n");
        printf("Note: The path parameter is the project root path\n");
        return 1;
    }

    engine_init(&rt, &ctx);

    int windowWidth = 600;
    int windowHeight = 300;
    Font *fonts = gui_init(windowWidth, windowHeight);
    js_init_module_gui(ctx, "GUI");

    char *scriptPath = args.path;

    char *totalPath = malloc(strlen(scriptPath) + strlen("/.internals/javascript/index.js"));
    strcpy(totalPath, scriptPath);
    strcat(totalPath, "/.internals/javascript/index.js");

    char *rawJS = LoadFileText(totalPath);
    JSValue ret = JS_Eval(ctx, rawJS, strlen(rawJS), totalPath, JS_EVAL_TYPE_MODULE);

    if (args.isWatch)
    {
        dmon_init();
        dmon_watch(scriptPath, watch_callback, 0, totalPath);
    }

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

    if (args.isWatch)
    {
        dmon_deinit();
        free(totalPath);
    }
    engine_deinit(ctx);
    gui_deinit(fonts);

    return 0;
}