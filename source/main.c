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
#include "events/keyboard.h"
#include "debug.h"
#include "platform.h"

extern void Clay_Raylib_Render(Clay_RenderCommandArray renderCommands, Font *fonts);

void idleCallback(uv_idle_t *handle)
{
    TJSRuntime *qrt = handle->data;
    JSContext *ctx = TJS_GetJSContext(qrt);

    if (WindowShouldClose() || JS_HasException(ctx))
    {
        CloseWindow();
        TJS_Stop(qrt);
        return;
    }

    Clay_SetPointerState((Clay_Vector2){GetMouseX(), GetMouseY()}, IsMouseButtonDown(MOUSE_BUTTON_LEFT));
    Clay_SetLayoutDimensions((Clay_Dimensions){GetRenderWidth(), GetRenderHeight()});

    Vector2 mouseWheelMove = GetMouseWheelMoveV();
    Clay_UpdateScrollContainers(true, (Clay_Vector2){.x = mouseWheelMove.x, .y = mouseWheelMove.y}, GetFrameTime());

    a_free();

    arenaIndex = (arenaIndex + 1) % 2;
    Clay_RenderCommandArray rc = GUI_RenderCommands(qrt);
    Font font = GetFontDefault();

    BeginDrawing();
    ClearBackground(BLANK);
    Clay_Raylib_Render(rc, &font);
    DrawFPS(10, 10);
    EndDrawing();

    EVENT_HandleEvents(ctx);
}

void HandleClayErrors(Clay_ErrorData errorData)
{
    // See the Clay_ErrorData struct for more information
    printf("%s\n", errorData.errorText.chars);
    exit(1);
}

int main(int argc, const char **argv)
{
    srand(123);
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
    Clay_Raylib_Initialize(500, 300, "Desktop.js", FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI);
    SetWindowOpacity(0);

    // Note: screenWidth and screenHeight will need to come from your environment, Clay doesn't handle window related tasks
    Clay_Initialize(arena, (Clay_Dimensions){GetScreenWidth(), GetScreenHeight()}, (Clay_ErrorHandler){HandleClayErrors});
    Font font = GetFontDefault();
    Clay_SetMeasureTextFunction(Raylib_MeasureText, &font);

    TJS_RunWithIdleCallback(qrt, idleCallback);

    JSRuntime *rt = JS_GetRuntime(ctx);
    JSMemoryUsage usage;
    JS_ComputeMemoryUsage(rt, &usage);
    printf("\nObject count:  %lld\nMem used size: %lld\nString count:  %lld\n", usage.obj_count, usage.memory_used_size, usage.str_count);

    TJS_FreeRuntime(qrt);
    free(clayArenaMemory);

    return 0;
}
