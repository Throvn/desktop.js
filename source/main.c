#include <pthread.h>
#include <stdlib.h>
#include "../lib/raylib/src/raylib.h"
#include "../lib/txiki.js/src/tjs.h"
#define CLAY_IMPLEMENTATION
#include "../lib/clay/clay.h"

#include "../lib/clay/renderers/raylib/clay_renderer_raylib.c"
#include "gui/js.h"
#include "gui/draw.h"
#include "events/mouse.h"
#include "debug.h"
#include "platform.h"

extern void Clay_Raylib_Render(Clay_RenderCommandArray renderCommands, Font *fonts);

void idleCallback(uv_idle_t *handle)
{
    TJSRuntime *qrt = handle->data;

    if (WindowShouldClose())
    {
        CloseWindow();
        TJS_Stop(qrt);
        return;
    }
    Clay_SetLayoutDimensions((Clay_Dimensions){GetRenderWidth(), GetRenderHeight()});
    Clay_SetPointerState((Clay_Vector2){GetMouseX(), GetMouseY()}, IsMouseButtonDown(MOUSE_BUTTON_LEFT));

    a_free();

    BeginDrawing();
    ClearBackground(BLACK);
    JSContext *ctx = TJS_GetJSContext(qrt);

    arenaIndex = (arenaIndex + 1) % 2;
    Clay_RenderCommandArray rc = GUI_RenderCommands(qrt);
    Font font = GetFontDefault();
    Clay_Raylib_Render(rc, &font);
    DrawFPS(10, 10);
    EndDrawing();
}

void HandleClayErrors(Clay_ErrorData errorData)
{
    // See the Clay_ErrorData struct for more information
    printf("%s", errorData.errorText.chars);
    exit(1);
}

int main(int argc, char **argv)
{
    // No need to free, because we need the result until shutdown.
    CliArgs *args = prepareArgs(argc, argv);
    // Init JS runtime
    TJS_Initialize(args->count, (char **)args->variables);

    TJSRuntime *qrt = TJS_NewRuntime();
    if (!qrt)
        return 1;

    // Load GUI import.
    JSContext *ctx = TJS_GetJSContext(qrt);
    GUI_js_init_module(ctx);

    uint64_t totalMemorySize = Clay_MinMemorySize();
    void *clayArenaMemory = malloc(totalMemorySize);
    Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, clayArenaMemory);

    SetTraceLogLevel(LOG_ERROR);
    Clay_Raylib_Initialize(500, 300, "Desktop.js", FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    SetWindowOpacity(0);

    // Note: screenWidth and screenHeight will need to come from your environment, Clay doesn't handle window related tasks
    Clay_Initialize(arena, (Clay_Dimensions){GetScreenWidth(), GetScreenHeight()}, (Clay_ErrorHandler){HandleClayErrors});
    Font font = GetFontDefault();
    Clay_SetMeasureTextFunction(Raylib_MeasureText, &font);

    TJS_RunWithIdleCallback(qrt, idleCallback);

    JSRuntime *rt = JS_GetRuntime(ctx);
    JSMemoryUsage usage;
    JS_ComputeMemoryUsage(rt, &usage);
    printf("Obj count: %lld\nMem used size: %lld\nStr count: %lld\n", usage.obj_count, usage.memory_used_size, usage.str_count);

    TJS_FreeRuntime(qrt);
    free(clayArenaMemory);

    return 0;
}
