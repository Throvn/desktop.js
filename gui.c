#include <stdlib.h>
#include "./lib/quickjs/quickjs.h"
#define CLAY_IMPLEMENTATION
#include "gui.h"

#include "./lib/clay/renderers/raylib/clay_renderer_raylib.c"

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
        exit(2);
    }
    // if (0 == strcmp(typeStr, "div"))
    {
        CLAY({
            .id = CLAY_ID("node->key"),
        })
        {
            CLAY_TEXT(CLAY_STRING("Cool"), CLAY_TEXT_CONFIG({.fontSize = 24, .textColor = {255, 255, 255, 255}}));
        };
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

    // Start building JSON obj similar to React.JSValue
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

static void js_element_class_finalizer(JSRuntime *rt, JSValue val)
{
    struct DOMNode *s = JS_GetOpaque(val, js_element_class_id);
    /* Note: 's' can be NULL in case JS_SetOpaque() was not called */
    free(s->type);
    free(s->descendants);
    js_free_rt(rt, s);
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
    printf("%s", errorData.errorText.chars);

    // TODO: Handle errors.
}

void gui_init(int width, int height)
{
    Clay_Raylib_Initialize(width, height, NULL, FLAG_WINDOW_RESIZABLE);
    ToggleBorderlessWindowed();
    ToggleBorderlessWindowed();

    uint64_t totalMemorySize = Clay_MinMemorySize();
    Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, malloc(totalMemorySize));

    Clay_Initialize(arena, (Clay_Dimensions){width, height}, (Clay_ErrorHandler){HandleClayErrors});
}

void gui_deinit()
{
    Clay_Raylib_Close();
}

Clay_RenderCommandArray gui_create_render_tree()
{
    const Clay_Color COLOR_LIGHT = (Clay_Color){224, 215, 210, 255};
    const Clay_Color COLOR_RED = (Clay_Color){168, 66, 28, 255};
    const Clay_Color COLOR_ORANGE = (Clay_Color){225, 138, 50, 255};

    Clay_BeginLayout();
    // An example of laying out a UI with a fixed width sidebar and flexible width main content
    CLAY({.id = CLAY_ID("OuterContainer"),
          .layout = {.layoutDirection = CLAY_LEFT_TO_RIGHT, // Add this for side-by-side layout
                     .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)},
                     .padding = CLAY_PADDING_ALL(16)},
          .backgroundColor = {250, 250, 255, 255}})
    {
        CLAY({.id = CLAY_ID("SideBar"),
              .layout = {.layoutDirection = CLAY_TOP_TO_BOTTOM,
                         .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)}}, // Fixed width, full height
              .backgroundColor = COLOR_LIGHT})
        {
            // Sidebar content goes here
        }

        CLAY({.id = CLAY_ID("MainContent"),
              .layout = {.sizing = {CLAY_SIZING_GROW(1), CLAY_SIZING_GROW(1)}}, // Grow to fill remaining space
              .backgroundColor = COLOR_RED})
        {
            CLAY_TEXT(
                CLAY_STRING("This is the main content"),
                CLAY_TEXT_CONFIG({
                    .fontSize = 20,
                    .textColor = {0, 0, 0, 255}, // Changed alpha to 255 for visible text
                }));
        }
    }

    Clay_RenderCommandArray renderCommands = Clay_EndLayout();
    return renderCommands;
}