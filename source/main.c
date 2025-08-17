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

    BeginDrawing();
    ClearBackground(BLACK);
    a_free();
    JSContext *ctx = TJS_GetJSContext(qrt);
    JS_RunGC(JS_GetRuntime(ctx));

    arenaIndex = (arenaIndex + 1) % 2;
    Clay_RenderCommandArray rc = GUI_RenderCommands(qrt);
    Font font = GetFontDefault();
    Clay_Raylib_Render(rc, &font);
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
    // Init JS runtime
    TJS_Initialize(argc, argv);

    TJSRuntime *qrt = TJS_NewRuntime();
    if (!qrt)
    {
        return 1;
    }

    // Load GUI import.
    JSContext *ctx = TJS_GetJSContext(qrt);
    GUI_js_init_module(ctx);

    SetTraceLogLevel(LOG_ERROR);
    InitWindow(300, 500, "Test Window");
    SetWindowState(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_HIGHDPI);

    // Otherwise text doesn't draw somehow.
    ToggleBorderlessWindowed();
    ToggleBorderlessWindowed();

    uint64_t totalMemorySize = Clay_MinMemorySize();
    void *clayArenaMemory = malloc(totalMemorySize);
    Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, clayArenaMemory);

    // Note: screenWidth and screenHeight will need to come from your environment, Clay doesn't handle window related tasks
    Clay_Initialize(arena, (Clay_Dimensions){GetScreenWidth(), GetScreenHeight()}, (Clay_ErrorHandler){HandleClayErrors});
    Font font = GetFontDefault();
    Clay_SetMeasureTextFunction(Raylib_MeasureText, &font);

    TJS_RunWithIdleCallback(qrt, idleCallback);

    TJS_FreeRuntime(qrt);
    free(clayArenaMemory);

    return 0;
}
