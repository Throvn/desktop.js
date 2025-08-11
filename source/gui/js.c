#include <quickjs.h>
#include <stdlib.h>

JSValue rootValue = JS_UNINITIALIZED;

// GUI.render()
static JSValue GUI_js_render(JSContext *ctx, JSValueConst this_val,
                             int argc, JSValueConst *argv)
{
    if (argc != 1)
    {
        JS_ThrowTypeError(ctx, "GUI.render() only takes one JSX element");
        return JS_UNDEFINED;
    }
    if (!JS_IsObject(argv[0]))
    {
        JS_ThrowTypeError(ctx, "GUI.render() accepts only objects");
        return JS_UNDEFINED;
    }

    printf("[GUI] render() called\n");
    rootValue = JS_DupValue(ctx, argv[0]);

    return JS_UNDEFINED;
}

static JSValue GUI_CreateBuiltInElement(JSContext *ctx, int argc, JSValueConst *argv)
{
    JSValue element = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, element, "type", JS_DupValue(ctx, argv[0]));
    JS_SetPropertyStr(ctx, element, "key", JS_NewInt64(ctx, rand()));

    JSValue props = JS_IsNull(argv[1]) ? JS_NewObject(ctx) : JS_DupValue(ctx, argv[1]);
    JS_SetPropertyStr(ctx, element, "props", props);

    JSValue children = JS_NewArray(ctx);
    for (int i = 0; i < argc - 2; i++)
    {
        JS_SetPropertyUint32(ctx, children, i, JS_DupValue(ctx, argv[2 + i]));
    }

    JS_SetPropertyStr(ctx, props, "children", children);

    return element;
}

static JSValue GUI_CreateCustomElement(JSContext *ctx, int argc, JSValueConst *argv)
{
    JSValue element = JS_NewObject(ctx);

    JS_SetPropertyStr(ctx, element, "type", JS_NewString(ctx, "custom"));
    JS_SetPropertyStr(ctx, element, "key", JS_NewInt64(ctx, rand()));

    JSValue props = JS_IsNull(argv[1]) ? JS_NewObject(ctx) : JS_DupValue(ctx, argv[1]);
    JS_SetPropertyStr(ctx, element, "props", props);

    JSValue children = JS_NewArray(ctx);
    for (int i = 0; i < argc - 2; i++)
    {
        JS_SetPropertyUint32(ctx, children, i, JS_DupValue(ctx, argv[2 + i]));
    }

    JS_SetPropertyStr(ctx, props, "children", children);

    JSValue instance = JS_CallConstructor(ctx, argv[0], 0, NULL);
    JS_SetPropertyStr(ctx, element, "instance", instance);

    return element;
}

static JSValue GUI_js_create_element(JSContext *ctx, JSValueConst this_val,
                                     int argc, JSValueConst *argv)
{
    if (argc < 2)
    {
        JS_ThrowTypeError(ctx, "[GUI] createElement() takes at least two parameters");
        return JS_UNDEFINED;
    }

    if (JS_IsString(argv[0]))
    {
        return GUI_CreateBuiltInElement(ctx, argc, argv);
    }
    else if (JS_IsFunction(ctx, argv[0]))
    {
        return GUI_CreateCustomElement(ctx, argc, argv);
    }
    else
    {
        printf("[GUI] createElement()s first parameter has an unknown type\n");
    }
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