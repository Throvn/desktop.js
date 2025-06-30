#include "../lib/quickjs/quickjs.h"
#include "../lib/quickjs/quickjs-libc.h"
#include "terminal_colors.h"

int engine_init(JSRuntime **rt, JSContext **ctx)
{
    *rt = JS_NewRuntime();
    if (*rt == NULL)
    {
        fprintf(stderr, "Cannot allocate runtime");
        return 0;
    }

    // JS_SetMemoryLimit(rt, 0x4000000);
    // JS_SetMaxStackSize(rt, 0x100000);

    *ctx = JS_NewContext(*rt);
    if (*ctx == NULL)
    {
        fprintf(stderr, "Cannot allocate context");
        return 0;
    }

    // Adds console.log to the global context
    js_std_init_handlers(*rt);
    js_std_add_helpers(*ctx, 0, NULL);

    // Adds file opration to the os context
    JSModuleDef *m = js_init_module_os(*ctx, "os");
    if (m == NULL)
    {
        fprintf(stderr, "Cannot create 'os' module");
        return 0;
    }

    // loader for ES6 modules
    JS_SetModuleLoaderFunc2(*rt, NULL, js_module_loader, js_module_check_attributes, NULL);
    return 1;
}

int engine_exception_handled(JSContext *ctx, JSValue scriptReturn)
{
    if (!JS_HasException(ctx))
    {
        return 0;
    }
    JSValue exception = JS_GetException(ctx);
    char *exceptionStr = JS_ToCString(ctx, exception);
    fprintf(stderr, BRED "%s\n" CRESET, exceptionStr);

    JSValue stack = JS_GetPropertyStr(ctx, exception, "stack");
    char *stackTraceStr = JS_ToCString(ctx, stack);
    fprintf(stderr, RED "%s\n" CRESET, stackTraceStr);

    return 1;
}

void engine_deinit(JSContext *ctx)
{
    fprintf(stderr, "Closing engine...");
    JSRuntime *rt = JS_GetRuntime(ctx);
    js_std_free_handlers(rt);
    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);
}