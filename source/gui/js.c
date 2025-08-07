#include <quickjs.h>

// GUI.render()
static JSValue GUI_js_render(JSContext *ctx, JSValueConst this_val,
                             int argc, JSValueConst *argv)
{
    printf("[GUI] render() called\n");
    return JS_UNDEFINED;
}

// GUI.createElement()
static JSValue GUI_js_create_element(JSContext *ctx, JSValueConst this_val,
                                     int argc, JSValueConst *argv)
{
    printf("[GUI] createElement() called\n");
    return JS_UNDEFINED;
}

// Module init function: defines exports
static int js_gui_init(JSContext *ctx, JSModuleDef *m)
{
    JS_SetModuleExport(ctx, m, "render",
                       JS_NewCFunction(ctx, GUI_js_render, "render", 0));
    JS_SetModuleExport(ctx, m, "createElement",
                       JS_NewCFunction(ctx, GUI_js_create_element, "createElement", 0));
    return 0;
}

// Required entry point for the module
JSModuleDef *GUI_js_init_module(JSContext *ctx)
{
    JSModuleDef *m = JS_NewCModule(ctx, "GUI", js_gui_init);
    if (!m)
        return NULL;
    JS_AddModuleExport(ctx, m, "render");
    JS_AddModuleExport(ctx, m, "createElement");
    return m;
}