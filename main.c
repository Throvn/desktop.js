#include <stdlib.h>
#include "quickjs.h"
#include "fetch.h"
#include "raylib.h"

int main()
{
    JSRuntime *rt = JS_NewRuntime();
    if (rt == NULL)
    {
        fprintf(stderr, "Cannot allocate runtime");
        exit(2);
    }

    JSContext *ctx = JS_NewContext(rt);
    if (ctx == NULL)
    {
        fprintf(stderr, "Cannot allocate context");
        exit(2);
    }

    InitWindow(300, 300, "Raylib");

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText("Congrats! You created your first window!", 20, 20, 20, LIGHTGRAY);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}