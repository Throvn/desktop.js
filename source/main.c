#include <pthread.h>
#include <stdlib.h>
#include "../lib/raylib/src/raylib.h"
#include "../lib/txiki.js/src/tjs.h"
#define CLAY_IMPLEMENTATION
#include "../lib/clay/clay.h"

#include "../lib/clay/renderers/raylib/clay_renderer_raylib.c"
#include "gui/js.h"

extern void Clay_Raylib_Render(Clay_RenderCommandArray renderCommands, Font *fonts);

void *js_start(void *qrt)
{
    // Load GUI import.
    JSContext *ctx = TJS_GetJSContext(qrt);
    GUI_js_init_module(ctx);

    // Blocking.
    TJS_Run(qrt);
    return NULL;
}

void HandleClayErrors(Clay_ErrorData errorData)
{
    // See the Clay_ErrorData struct for more information
    printf("%s", errorData.errorText.chars);
    exit(1);
}

int main(int argc, char **argv)
{

    TJS_Initialize(argc, argv);

    TJSRuntime *qrt = TJS_NewRuntime();
    if (!qrt)
    {
        return 1;
    }

    pthread_t js_thread;
    pthread_create(&js_thread, NULL, js_start, qrt);

    SetTraceLogLevel(LOG_ERROR);
    InitWindow(600, 300, "Test Window");

    // Otherwise text doesn't draw somehow.
    ToggleBorderlessWindowed();
    ToggleBorderlessWindowed();

    uint64_t totalMemorySize = Clay_MinMemorySize();
    Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, malloc(totalMemorySize));

    // Note: screenWidth and screenHeight will need to come from your environment, Clay doesn't handle window related tasks
    Clay_Initialize(arena, (Clay_Dimensions){GetScreenWidth(), GetScreenHeight()}, (Clay_ErrorHandler){HandleClayErrors});
    Font font = GetFontDefault();
    Clay_SetMeasureTextFunction(Raylib_MeasureText, &font);

    while (!WindowShouldClose())
    {
        Clay_SetLayoutDimensions((Clay_Dimensions){GetRenderWidth(), GetRenderHeight()});

        Font font = GetFontDefault();
        printf("Default font: texture.id=%u baseSize=%d glyphCount=%d\n",
               font.texture.id, font.baseSize, font.glyphCount);
        ClearBackground(WHITE);
        Clay_BeginLayout();

        CLAY((Clay_ElementDeclaration){
            .layout = {
                .sizing = {
                    CLAY_SIZING_GROW(),
                    CLAY_SIZING_GROW(),
                },
            },
        })
        {
            CLAY_TEXT(CLAY_STRING("This is a clay test"), CLAY_TEXT_CONFIG((Clay_TextElementConfig){
                                                              .fontSize = 24,
                                                              .textColor = (Clay_Color){0, 0, 0, 255},
                                                              .letterSpacing = 5,
                                                          }));
        }

        Clay_RenderCommandArray rc = Clay_EndLayout();
        Clay_Raylib_Render(rc, &font);
        EndDrawing();
    }

    pthread_cancel(js_thread);

    CloseWindow();
    TJS_FreeRuntime(qrt);

    return 0;
}