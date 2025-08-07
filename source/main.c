#include <pthread.h>
#include <stdlib.h>
#include "../lib/raylib/src/raylib.h"
#include "../lib/txiki.js/src/tjs.h"

#include "../lib/clay/clay.h"

extern void Clay_Raylib_Render(Clay_RenderCommandArray renderCommands, Font *fonts);
JSModuleDef *js_init_module_gui(JSContext *ctx, const char *module_name);

void *js_start(void *qrt)
{
    TJS_Run(qrt);
}

int main(int argc, char **argv)
{

    TJS_Initialize(argc, argv);

    TJSRuntime *qrt = TJS_NewRuntime();
    if (!qrt)
    {
        CloseWindow();
        return 1;
    }

    pthread_t js_thread;
    pthread_create(&js_thread, NULL, js_start, qrt);

    InitWindow(600, 300, "Test Window");

    JSContext *ctx = TJS_GetJSContext(qrt);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Congrats! You created your first window!", 190, 100, 20, LIGHTGRAY);
        EndDrawing();
    }

    pthread_cancel(js_thread);

    TJS_FreeRuntime(qrt);

    return 0;
}