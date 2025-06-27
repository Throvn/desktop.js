#include <stdlib.h>
#include "quickjs.h"
#include "quickjs-libc.h"
#include "minnet.h"
#include "raylib.h"
#include "gui.h"

int main()
{
    JSRuntime *rt = JS_NewRuntime();
    if (rt == NULL)
    {
        fprintf(stderr, "Cannot allocate runtime");
        exit(2);
    }
    // JS_SetMemoryLimit(rt, 0x4000000);
    // JS_SetMaxStackSize(rt, 0x100000);

    JSContext *ctx = JS_NewContext(rt);
    if (ctx == NULL)
    {
        fprintf(stderr, "Cannot allocate context");
        exit(2);
    }

    js_std_init_handlers(rt);
    // Needed to add console.log to the global context
    js_std_add_helpers(ctx, 0, NULL);
    js_init_module_os(ctx, "os");

    // loader for ES6 modules
    JS_SetModuleLoaderFunc2(rt, NULL, js_module_loader, js_module_check_attributes, NULL);

    InitWindow(600, 300, "Raylib");

    // Fix to kick off drawing cycle. Otherwise no drawing is done.
    ToggleBorderlessWindowed();
    ToggleBorderlessWindowed();

    char *rawJS = LoadFileText("./test.js");
    JSValue ret = JS_Eval(ctx, rawJS, strlen(rawJS), "", JS_EVAL_TYPE_MODULE);

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

    while (!WindowShouldClose())
    {
        BeginDrawing();

        // ClearBackground(RAYWHITE);

        DrawText("Congrats! You created your first window!", 20, 20, 10, LIGHTGRAY);
        DrawFPS(20, 20);

        EndDrawing();
    }
    CloseWindow();

    js_std_free_handlers(rt);
    JS_FreeContext(ctx);
    return 0;
}