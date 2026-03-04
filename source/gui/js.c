#include <quickjs.h>
#include <stdlib.h>
#include "../debug.h"
#include "raylib.h"
#include "js.h"

/// @brief Used to give js elements continuous ids
uint32_t idCounter = 0;
/// @brief Holds the value of the JSX root node.
JSValue rootValue = JS_UNINITIALIZED;
/// @brief Holds the value of the currently focused element.
/// @brief This is also known as the `document.activeElement` property in browsers.
JSValue focusValue = JS_UNINITIALIZED;

static void GUI_SetWindowOptions(JSContext *ctx, JSValueConst options)
{
    // Set window size.
    JSValue widthValue = JS_GetPropertyStr(ctx, options, "width");
    int width = 500;
    if (JS_IsNumber(widthValue))
        JS_ToInt32(ctx, &width, widthValue);
    JS_FreeValue(ctx, widthValue);
    JSValue heightValue = JS_GetPropertyStr(ctx, options, "height");
    int height = 300;
    if (JS_IsNumber(heightValue))
        JS_ToInt32(ctx, &height, heightValue);
    JS_FreeValue(ctx, heightValue);

    SetWindowSize(width, height);

    // Set window title.
    JSValue titleValue = JS_GetPropertyStr(ctx, options, "title");
    if (JS_IsString(titleValue))
    {
        const char *title = JS_ToCString(ctx, titleValue);
        SetWindowTitle(title);
        JS_FreeCString(ctx, title);
    }

    JSValue resizableValue = JS_GetPropertyStr(ctx, options, "resizable");
    if (JS_IsBool(resizableValue) && JS_ToBool(ctx, resizableValue))
    {
        SetWindowState(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI);
    }

    SetWindowOpacity(1);

    JS_FreeValue(ctx, titleValue);
    JS_FreeValue(ctx, heightValue);
    JS_FreeValue(ctx, widthValue);
}

// GUI.render()
static JSValue GUI_js_render(JSContext *ctx, JSValueConst this_val,
                             int argc, JSValueConst *argv)
{
    if (argc != 1 && argc != 2)
    {
        JS_ThrowTypeError(ctx, "GUI.render() only takes one JSX element and an optional options object");
        return JS_UNDEFINED;
    }
    if (!JS_IsObject(argv[0]))
    {
        JS_ThrowTypeError(ctx, "GUI.render() accepts only objects");
        return JS_UNDEFINED;
    }
    if (argc == 2 && !JS_IsObject(argv[1]))
    {
        JS_ThrowTypeError(ctx, "GUI.render() did not receive an options object as a second parameter");
        return JS_UNDEFINED;
    }
    GUI_SetWindowOptions(ctx, argv[1]);

    printf("[GUI] render() called\n");
    rootValue = JS_DupValue(ctx, argv[0]);

    focusValue = GUI_CreateRootElement(ctx);

    return JS_UNDEFINED;
}

/// @brief Turns any element to a GUI String element by using the string representation of the given element.
/// @param ctx
/// @param string Any JSValue you want to represent as a string in the GUI.
/// @return GUI Element (JS Object)
static JSValue GUI_ToStringElement(JSContext *ctx, JSValueConst string)
{
    JSValue element = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, element, "type", JS_NewString(ctx, "string"));
    JSValue props = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, element, "props", props);

    JSValue child;
    if (JS_IsString(string) || JS_IsNumber(string))
        child = JS_ToString(ctx, string);
    else
        child = JS_NewString(ctx, ""); // don't show stuff like booleans, null and so on.
    JS_SetPropertyStr(ctx, props, "children", child);

    return element;
}

static JSValue GUI_CreateBuiltInElement(JSContext *ctx, int argc, JSValueConst *argv)
{
    JSValue element = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, element, "type", JS_DupValue(ctx, argv[0]));

    JSValue props = JS_IsNull(argv[1]) ? JS_NewObject(ctx) : JS_DupValue(ctx, argv[1]);
    JS_SetPropertyStr(ctx, element, "props", props);

    JSValue children = JS_NewArray(ctx);
    for (int i = 0; i < argc - 2; i++)
    {
        JSValue child = JS_DupValue(ctx, argv[2 + i]);
        if (!JS_IsObject(argv[2 + i]))
        {
            child = GUI_ToStringElement(ctx, child);
        }
        JS_SetPropertyUint32(ctx, children, i, child);
    }

    JS_SetPropertyStr(ctx, props, "children", children);

    return element;
}

static JSValue GUI_CreateCustomElement(JSContext *ctx, int argc, JSValueConst *argv)
{
    JSValue element = JS_NewObject(ctx);

    JS_SetPropertyStr(ctx, element, "type", JS_NewString(ctx, "custom"));

    JSValue props = JS_IsNull(argv[1]) ? JS_NewObject(ctx) : JS_DupValue(ctx, argv[1]);
    JS_SetPropertyStr(ctx, element, "props", props);

    JSValue children = JS_NewArray(ctx);
    for (int i = 0; i < argc - 2; i++)
    {
        JSValue child = JS_DupValue(ctx, argv[2 + i]);
        if (JS_IsString(argv[2 + i]))
        {
            child = GUI_ToStringElement(ctx, child);
        }
        JS_SetPropertyUint32(ctx, children, i, child);
    }

    JS_SetPropertyStr(ctx, props, "children", children);

    if (!JS_IsConstructor(ctx, argv[0]))
    {
        fprintf(stderr, "[GUI] Instantiation of custom element failed. Element is not a class.\n");
        exit(5);
        return JS_UNDEFINED;
    }
    JS_SetPropertyStr(ctx, element, "class", JS_DupValue(ctx, argv[0]));
    JS_SetPropertyStr(ctx, element, "instance", JS_UNDEFINED);
    JS_SetPropertyStr(ctx, element, "_renderChild", JS_UNDEFINED);

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

    JSValue element = JS_UNDEFINED;
    if (JS_IsString(argv[0]))
        element = GUI_CreateBuiltInElement(ctx, argc, argv);
    else if (JS_IsFunction(ctx, argv[0]))
        element = GUI_CreateCustomElement(ctx, argc, argv);
    else
        fprintf(stderr, "[GUI] createElement()s first parameter has an unknown type\n");

    // Give each element a random key. (Needed for mouse events)
    JS_SetPropertyStr(ctx, element, "key", JS_NewUint32(ctx, idCounter++));

    return element;
}

// Module init function: defines exports
static int js_gui_init(JSContext *ctx, JSModuleDef *m)
{
    JS_SetModuleExport(ctx, m, "render",
                       JS_NewCFunction(ctx, GUI_js_render, "render", 1));
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

/**
 * @brief Creates a dummy JSX element of type "root".
 *
 * @param ctx
 * @return JSValue the "root" JSX object.
 */
JSValue GUI_CreateRootElement(JSContext *ctx)
{
    JSValue obj = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, obj, "type", JS_NewString(ctx, "root"));
    JS_SetPropertyStr(ctx, obj, "props", JS_NewArray(ctx));

    return obj;
}