#define CLAY_IMPLEMENTATION
#include "gui.h"
#include "styles.h"

#include "../lib/clay/renderers/raylib/clay_renderer_raylib.c"
#include "draw.h"

#define countof(x) (sizeof(x) / sizeof((x)[0]))

#ifdef JS_SHARED_LIBRARY
#define JS_INIT_MODULE js_init_module
#else
#define JS_INIT_MODULE js_init_module_gui
#endif

static JSClassID js_element_class_id;

static struct DOMNode *rootNode = NULL;

static JSValue render(JSContext *ctx, JSValue this_func, int argc, JSValueConst argv[])
{
    fprintf(stdout, "Starting render...\n");

    struct DOMNode *potentialNode = JS_GetOpaque(argv[0], js_element_class_id);
    if (potentialNode == NULL)
    {
        fprintf(stderr, "[render] Could not find DOMNode from JS render value");
        exit(1);
    }

    rootNode = potentialNode;
    return JS_TRUE;
}

static struct DOMNode *createStringNode(JSContext *ctx, JSValue string)
{
    if (false == JS_IsString(string) && false == JS_IsNumber(string))
        return NULL;

    // Store the value of the string in the properties object.
    JSValue prop = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, prop, "value", JS_DupValue(ctx, string));

    struct DOMNode *node = calloc(1, sizeof(struct DOMNode));
    node->ctx = ctx;
    node->type = "string";
    node->key = CLAY_IDI("", rand());
    node->properties = prop;

    return node;
}

JSValue createChildren(JSContext *ctx, int argc, JSValue argv[], struct DOMNode ***descendants_out)
{
    JSValue children = JS_NewArray(ctx);

    // Allocate a temporary buffer (we will realloc later)
    int capacity = argc;
    struct DOMNode **descendants = malloc(capacity * sizeof(struct DOMNode *));
    int count = 0;
    int childIndex = 0;

    for (int i = 0; i < argc; i++)
    {
        JSValue current = argv[i];

        if (JS_IsArray(ctx, current))
        {
            JSValue lenValue = JS_GetPropertyStr(ctx, current, "length");
            int arrLen = 0;
            JS_ToInt32(ctx, &arrLen, lenValue);
            JS_FreeValue(ctx, lenValue);

            JSValue *arrContents = malloc(arrLen * sizeof(JSValue));
            for (int j = 0; j < arrLen; j++)
                arrContents[j] = JS_GetPropertyUint32(ctx, current, j);

            struct DOMNode **nestedDescendants = NULL;
            JSValue nestedChildren = createChildren(ctx, arrLen, arrContents, &nestedDescendants);

            // Flatten returned JSValue array
            for (int j = 0; j < arrLen; j++)
            {
                JSValue val = JS_GetPropertyUint32(ctx, nestedChildren, j);
                JS_SetPropertyUint32(ctx, children, childIndex++, val);
                JS_FreeValue(ctx, val); // cleanup temporary reference
            }

            // Merge nested descendants
            for (int j = 0; j < arrLen; j++)
            {
                if (count >= capacity)
                {
                    capacity *= 2;
                    descendants = realloc(descendants, capacity * sizeof(struct DOMNode *));
                }
                descendants[count++] = nestedDescendants[j];
            }

            free(arrContents);
            free(nestedDescendants);
            JS_FreeValue(ctx, nestedChildren);
        }
        else
        {
            struct DOMNode *node = JS_GetOpaque(current, js_element_class_id);
            if (!node)
                node = createStringNode(ctx, current);

            if (count >= capacity)
            {
                capacity *= 2;
                descendants = realloc(descendants, capacity * sizeof(struct DOMNode *));
            }

            descendants[count++] = node;
            JS_SetPropertyUint32(ctx, children, childIndex++, JS_DupValue(ctx, current));
        }
    }

    *descendants_out = descendants;
    return children;
}

/// @brief Turns the component into React component json (Like real react), and creates a DOMNode element for drawing later.
/// @brief Called from JS!
/// @param ctx QuickJS Context
/// @param this_val QuickJS function (React.createElement)
/// @param argc Number of supplied JS arguments
/// @param argv the JS arguments itself
/// @return A JS Object which contains all of the JSX information.
static JSValue createElement(JSContext *ctx, JSValue this_val, int argc, JSValueConst argv[])
{
    JSValueConst componentView = argv[0];
    char *componentName = JS_ToCString(ctx, componentView);
    // fprintf(stdout, "Wants to create a new %s\n", componentName);

    // Unique id to recognize this component.
    int key = rand();

    JSValue props = JS_IsNull(argv[1]) ? JS_NewObject(ctx) : JS_DupValue(ctx, argv[1]);

    int num_descendants = argc - 2;
    struct DOMNode **descendants;
    struct JSValue children;

    if (JS_IsFunction(ctx, componentView))
    {
        // Call constructor of custom component
        JSValue propArray[] = {props};
        JSValue instance = JS_CallConstructor(ctx, componentView, 1, propArray);

        // Add the instance to properties, so we can call the render functions repeatedly later on.
        JS_SetPropertyStr(ctx, props, "instance", instance);

        // TODO: Register component and its functions and stuff.

        // Call render function
        int hasRenderFunction = JS_HasProperty(ctx, instance, JS_NewAtom(ctx, "render"));
        if (hasRenderFunction == true)
        {
            JSValue jsRenderFunction = JS_GetPropertyStr(ctx, instance, "render");
            JSValue jsRenderReturn = JS_Call(ctx, jsRenderFunction, instance, 0, NULL);
            num_descendants = 1;
            JSValue elements[] = {jsRenderReturn};
            children = createChildren(ctx, num_descendants, elements, &descendants);
        }
        else if (hasRenderFunction == false)
        {
            fprintf(stderr, "Component does not have render() function");
            return JS_ThrowTypeError(ctx, "your custom component doesn't have a render() function but was used in your JSX tree.");
        }
        else
        {
            return JS_ThrowInternalError(ctx, "lookup for the render function failed.");
        }
    }
    else
    {
        children = createChildren(ctx, num_descendants, &argv[2], &descendants);
    }

    // Start building JSON obj similar to React.JSValue
    JSValue ret = JS_NewObjectClass(ctx, js_element_class_id);
    JS_SetPropertyStr(ctx, ret, "type", JS_DupValue(ctx, componentView));
    JS_SetPropertyStr(ctx, ret, "key", JS_NewInt32(ctx, key));

    struct DOMNode *domNode = calloc(1, sizeof(struct DOMNode));
    JS_SetPropertyStr(ctx, props, "children", children);
    JS_SetPropertyStr(ctx, ret, "props", JS_DupValue(ctx, props));

    domNode->type = strdup(componentName);
    domNode->key = CLAY_IDI("", key);

    // Get the length of the flattened children:
    JSValue new_num_descendants = JS_GetPropertyStr(ctx, children, "length");
    JS_ToInt32(ctx, &domNode->num_descendants, new_num_descendants);

    domNode->descendants = descendants;

    domNode->ctx = ctx;
    domNode->properties = JS_DupValue(ctx, props);
    domNode->styles = getAllStyles(domNode);

    JS_SetOpaque(ret, domNode);
    return ret;
}

/// @brief Handles cleanup when JSX components go out of JS scope.
/// @param rt QuickJS Runtime
/// @param val JSValue which goes out of Scope
static void js_element_class_finalizer(JSRuntime *rt, JSValue val)
{
    struct DOMNode *s = JS_GetOpaque(val, js_element_class_id);
    /* Note: 's' can be NULL in case JS_SetOpaque() was not called */
    // free(s->type);
    // free(s->descendants);
    // js_free_rt(rt, s);
    // TODO: Clean up memory accordingly!
    // TODO: Make sure that there are no dangling pointers left.
}
static JSClassDef js_element_class = {
    .class_name = "Element",
    .finalizer = js_element_class_finalizer,
};

/// @brief Initializes all classes whic are needed for the gui JS module
/// @param ctx QuickJS context
static void initGuiClasses(JSContext *ctx)
{
    JS_NewClassID(&js_element_class_id);
    JS_NewClass(JS_GetRuntime(ctx), js_element_class_id, &js_element_class);
    printf("initGuiClasses\n");
    // TODO: Add Component class (similar to React.Component)
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
    printf("LIBGUI initializing...\n");
    // Create new module
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_gui_init);

    if (!m)
        return NULL;

    initGuiClasses(ctx);
    JS_AddModuleExportList(ctx, m, js_gui_functions, countof(js_gui_functions));
    return m;
}

////////////////
// Clay Stuff //
////////////////

void HandleClayErrors(Clay_ErrorData errorData)
{
    // See the Clay_ErrorData struct for more information
    fprintf(stderr, "%s", errorData.errorText.chars);
    exit(1000);
    // TODO: Handle errors.
}

Font *gui_init(int width, int height)
{
    Clay_Raylib_Initialize(width, height, NULL, FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    ToggleBorderlessWindowed();
    ToggleBorderlessWindowed();

    uint64_t totalMemorySize = Clay_MinMemorySize();
    Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, malloc(totalMemorySize));

    Clay_Initialize(arena, (Clay_Dimensions){width, height}, (Clay_ErrorHandler){HandleClayErrors});

    Font *fonts = malloc(sizeof(Font[1]));
    fonts[0] = LoadFontEx("../assets/fonts/Roboto-Regular.ttf", 48, 0, 0);
    SetTextureFilter(fonts[0].texture, TEXTURE_FILTER_BILINEAR);
    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

    return fonts;
}

void gui_deinit(Font *fonts)
{
    free(fonts);
    Clay_Raylib_Close();
}

Clay_RenderCommandArray gui_create_render_tree(JSContext *ctx)
{
    Clay_BeginLayout();

    if (rootNode == NULL)
    {
        return Clay_EndLayout();
    }

    CLAY((Clay_ElementDeclaration){
        .backgroundColor = {255, 255, 255, 255},
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)},
            .childAlignment = {
                CLAY_ALIGN_X_CENTER,
                CLAY_ALIGN_Y_CENTER,
            },
        },
    })
    {
        if (rootNode != NULL)
        {
            renderElement(rootNode);
        }
    }

    Clay_RenderCommandArray renderCommands = Clay_EndLayout();
    return renderCommands;
}

JSValue createMouseEvent(struct DOMNode *node)
{
    JSValue mouseEvent = JS_NewObject(node->ctx);

    // Add altKey property
    int isAltKeyPressed = IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT);
    JSValue isAltKeyPressedJSValue = JS_NewBool(node->ctx, isAltKeyPressed);
    JS_SetPropertyStr(node->ctx, mouseEvent, "altKey", isAltKeyPressedJSValue);

    int isShiftKeyPressed = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    JSValue isShiftKeyPressedJSValue = JS_NewBool(node->ctx, isShiftKeyPressed);
    JS_SetPropertyStr(node->ctx, mouseEvent, "shiftKey", isShiftKeyPressedJSValue);

    int isCtrlKeyPressed = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
    JSValue isCtrlKeyPressedJSValue = JS_NewBool(node->ctx, isCtrlKeyPressed);
    JS_SetPropertyStr(node->ctx, mouseEvent, "ctrlKey", isCtrlKeyPressedJSValue);

    // Add coordinates
    Vector2 screenMousePosition = GetMousePosition();
    JSValue xScreenCoordinate = JS_NewInt32(node->ctx, screenMousePosition.x);
    JS_SetPropertyStr(node->ctx, mouseEvent, "layerX", xScreenCoordinate);
    JSValue yScreenCoordinate = JS_NewInt32(node->ctx, screenMousePosition.y);
    JS_SetPropertyStr(node->ctx, mouseEvent, "layerY", yScreenCoordinate);

    return mouseEvent;
}

int fireMouseOver(struct DOMNode *node)
{
    Vector2 delta = GetMouseDelta();
    if (0 == delta.x + delta.y)
        return 0;

    JSValue mouseOverValue = JS_GetPropertyStr(node->ctx, node->properties, "onMouseOver");
    if (!JS_IsFunction(node->ctx, mouseOverValue))
    {
        return 0;
    }

    JSValue mouseEvent = createMouseEvent(node);
    JS_Call(node->ctx, mouseOverValue, mouseOverValue, 1, &mouseEvent);
    JS_FreeValue(node->ctx, mouseEvent);

    return 1;
}

int fireMouseDown(struct DOMNode *node)
{
    static bool wasButtonPressed = false;
    bool isButtonPressed = IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT) || IsMouseButtonDown(MOUSE_BUTTON_MIDDLE);
    if (wasButtonPressed == isButtonPressed)
        return 0;

    wasButtonPressed = isButtonPressed;

    if (!isButtonPressed)
        return 0;

    // Check that the node is listening for this event.
    JSValue mouseDownValue = JS_GetPropertyStr(node->ctx, node->properties, "onMouseDown");
    printf("This is not a mouseDownValue %s\n", node->type);
    if (!JS_IsFunction(node->ctx, mouseDownValue))
        return 0;

    printf("Was button pressed: %i \n", isButtonPressed);

    JSValue mouseEvent = createMouseEvent(node);

    // Add which button was pressed
    int button = IsMouseButtonDown(MOUSE_BUTTON_MIDDLE) ? 1 : (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) ? 2 : 0);
    JSValue buttonValue = JS_NewInt32(node->ctx, button);
    JS_SetPropertyStr(node->ctx, mouseEvent, "button", buttonValue);

    JS_Call(node->ctx, mouseDownValue, mouseDownValue, 1, &mouseEvent);
    JS_FreeValue(node->ctx, mouseEvent);

    return 1;
}

int fireMouseEvents(struct DOMNode *node)
{
    if (!Clay_PointerOver(node->key))
        return 0;

    fireMouseDown(node);
    fireMouseOver(node);
}

struct DOMNode *findNodeByKey(struct DOMNode *root, Clay_ElementId key)
{
    if (root == NULL)
        return NULL;

    if (root->key.id == key.id)
    {
        return root;
    }

    for (int i = 0; i < root->num_descendants; i++)
    {
        struct DOMNode *node = findNodeByKey(root->descendants[i], key);
        if (node != NULL)
            return node;
    }

    return NULL;
}

void gui_fire_events()
{
    if (rootNode == NULL)
        return;

    Clay_ElementIdArray ids = Clay_GetPointerOverIds();
    if (ids.length < 1)
        return;

    // Start from smallest node first (most nested element, gets mouse events first)
    for (int i = ids.length; 0 <= i; i--)
    {
        struct DOMNode *node = findNodeByKey(rootNode, ids.internalArray[i]);
        if (node == NULL)
            continue;

        fireMouseEvents(node);
    }
}