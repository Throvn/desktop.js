#include "../lib/quickjs/quickjs.h"
#include "../lib/quickjs/quickjs-libc.h"

int engine_init(JSRuntime **rt, JSContext **ctx)
{
    *rt = JS_NewRuntime();
    if (*rt == NULL)
    {
        fprintf(stderr, "Cannot allocate runtime");
        return 1;
    }

    // JS_SetMemoryLimit(rt, 0x4000000);
    // JS_SetMaxStackSize(rt, 0x100000);

    *ctx = JS_NewContext(*rt);
    if (*ctx == NULL)
    {
        fprintf(stderr, "Cannot allocate context");
        return 2;
    }

    // Adds console.log to the global context
    js_std_init_handlers(*rt);
    js_std_add_helpers(*ctx, 0, NULL);

    // Adds file opration to the os context
    JSModuleDef *m = js_init_module_os(*ctx, "os");
    if (m == NULL)
    {
        fprintf(stderr, "Cannot create 'os' module");
        return 3;
    }

    // loader for ES6 modules
    JS_SetModuleLoaderFunc2(*rt, NULL, js_module_loader, js_module_check_attributes, NULL);
    return 0;
}

void engine_deinit(JSContext *ctx)
{
    fprintf(stderr, "Closing engine...");
    JSRuntime *rt = JS_GetRuntime(ctx);
    js_std_free_handlers(rt);
    JS_FreeContext(ctx);
}