#include <stdlib.h>
#include "./lib/raylib/raylib/include/raylib.h"
#include "./lib/quickjs/quickjs.h"
#include "gui.h"

#define countof(x) (sizeof(x) / sizeof((x)[0]))

#ifdef JS_SHARED_LIBRARY
#define JS_INIT_MODULE js_init_module
#else
#define JS_INIT_MODULE js_init_module_gui
#endif

static JSClassID js_element_class_id;

static int renderElement(JSContext *ctx, JSValue comp, int posX, int posY, int width, int height)
{
    JSValue typeVal = JS_GetPropertyStr(ctx, comp, "type");
    char *typeStr = JS_ToCString(ctx, typeVal);
    struct DOMNode *node = JS_GetOpaque(comp, js_element_class_id);
    if (node == NULL)
    {
        fprintf(stderr, "Could not get attached C struct from element\n");
        return -1;
        // exit(2);
    }
    fprintf(stdout, "Key from C struct: %d", node->key);
    // if (0 == strcmp(typeStr, "div"))
    {
        DrawRectangle(50, 50, 100, 100, WHITE);
        DrawRectangleLines(posX - 1, posY - 1, width - 2, height - 2, WHITE);
    }
    return 0;
}

static JSValue render(JSContext *ctx, JSValue this_func, int argc, JSValueConst argv[])
{
    fprintf(stdout, "Starting render...\n");
    int width = GetRenderWidth();
    int height = GetRenderHeight();

    int status = renderElement(ctx, argv[0], 20, 20, width - 20, height - 20);
    if (status != 0)
    {
        fprintf(stderr, "Status was: %d\n", status);
    }
    return JS_FALSE;
}

// Turns the component into React component json (Like real react)
static JSValue createElement(JSContext *ctx, JSValue this_val, int argc, JSValueConst argv[])
{
    // Unique id to recognize this component.
    int key = rand();

    JSValueConst componentView = argv[0];
    char *componentName = JS_ToCString(ctx, componentView);

    fprintf(stdout, "Wants to create a new %s\n", componentName);

    // Start building JSON obj similar to React.
    JSValue ret = JS_NewObjectClass(ctx, js_element_class_id);
    JS_SetPropertyStr(ctx, ret, "type", JS_DupValue(ctx, componentView));
    JS_SetPropertyStr(ctx, ret, "key", JS_NewInt32(ctx, key));

    JSValue props = JS_IsNull(argv[1]) ? JS_NewObject(ctx) : JS_DupValue(ctx, argv[1]);

    JSValue children = JS_NewArray(ctx);
    struct DOMNode **descendants = calloc(argc - 2, sizeof(struct DOMNode *));
    for (int i = 2; i < argc; i++)
    {
        struct DOMNode *descendant = JS_GetOpaque(argv[i], js_element_class_id);
        descendants[i - 2] = descendant;
        JS_SetPropertyUint32(ctx, children, i - 2, JS_DupValue(ctx, argv[i]));
    };
    JS_SetPropertyStr(ctx, props, "children", children);

    JS_SetPropertyStr(ctx, ret, "props", props);

    struct DOMNode *domNode = calloc(1, sizeof(struct DOMNode));
    domNode->type = strdup(componentName);
    domNode->key = key;
    domNode->num_descendants = argc - 2;
    domNode->descendants = descendants;

    JS_SetOpaque(ret, domNode);
    return ret;
}

static JSClassID js_component_class_id;

static void js_element_class_finalizer(JSRuntime *rt, JSValue val)
{
    struct DOMNode *s = JS_GetOpaque(val, js_component_class_id);
    /* Note: 's' can be NULL in case JS_SetOpaque() was not called */
    js_free_rt(rt, s);
}
static JSClassDef js_element_class = {
    .class_name = "Element",
    .finalizer = js_element_class_finalizer,
};

// Pretty much same as Element. The difference between an Element and a Component is that an Element is built in.
static JSClassDef js_component_class = {
    .class_name = "Component",
    .finalizer = js_element_class_finalizer,
};

static void initGuiClasses(JSContext *ctx)
{
    JS_NewClassID(&js_element_class_id);
    JS_NewClass(JS_GetRuntime(ctx), js_element_class_id, &js_element_class);

    JS_NewClassID(&js_component_class_id);
    JS_NewClass(JS_GetRuntime(ctx), js_component_class_id, &js_component_class);
}

static JSCFunctionListEntry js_gui_functions[] = {
    JS_CFUNC_DEF("createElement", 3, createElement),
    JS_CFUNC_DEF("render", 1, render),
};

int js_gui_init(JSContext *ctx, JSModuleDef *m)
{
    return JS_SetModuleExportList(ctx, m, js_gui_functions, countof(js_gui_functions));
}

JSModuleDef *JS_INIT_MODULE(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_gui_init);

    if (!m)
        return NULL;

    initGuiClasses(ctx);
    JS_AddModuleExportList(ctx, m, js_gui_functions, countof(js_gui_functions));
    return m;
}
