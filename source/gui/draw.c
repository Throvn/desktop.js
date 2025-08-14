#include "../events/mouse.h"
#include "stdlib.h"
#include "draw.h"
#include "styles.h"

/// @brief Defined in js.c
extern JSValue rootValue;

void GUI_PrintKeys(JSContext *ctx, JSValue element)
{
    if (!JS_IsObject(element))
    {
        printf("[GUI_PrintKeys] Not an object\n");
        return;
    }

    JSPropertyEnum *props = NULL;
    uint32_t len = 0;
    int flags = JS_GPN_STRING_MASK | JS_GPN_SYMBOL_MASK;

    if (JS_GetOwnPropertyNames(ctx, &props, &len, element, flags) < 0)
    {
        printf("[GUI_PrintKeys] Failed to get property names\n");
        return;
    }

    for (uint32_t i = 0; i < len; i++)
    {
        JSValue key_val = JS_AtomToValue(ctx, props[i].atom);
        const char *key_str = JS_ToCString(ctx, key_val);
        if (key_str)
        {
            printf("Key[%u]: %s\n", i, key_str);
            JS_FreeCString(ctx, key_str);
        }
        JS_FreeAtom(ctx, props[i].atom);
    }

    js_free(ctx, props);
}

/// Checks if the JSValue could be an element by checking, if
/// the JSValue includes all necessary parameters and their expected types.
bool GUI_IsElement(JSContext *ctx, JSValue element)
{
    if (!JS_IsObject(element))
        return false;
    JSValue key = JS_GetPropertyStr(ctx, element, "key");
    if (!JS_IsNumber(key))
        return false;
    JSValue props = JS_GetPropertyStr(ctx, element, "props");
    if (!JS_IsArray(ctx, props))
        return false;
    JSValue type = JS_GetPropertyStr(ctx, element, "type");
    if (!JS_IsString(type))
        return false;

    return true;
}

/// @brief Gives back the value of the length property of the passed element.
int GUI_GetLength(JSContext *ctx, JSValue element)
{
    if (!JS_IsObject(element))
    {
        return -1;
    }

    JSValue lengthValue = JS_GetPropertyStr(ctx, element, "length");
    if (JS_IsUndefined(lengthValue))
    {
        GUI_PrintKeys(ctx, element);
        exit(1);
        return -1;
    }
    int length;
    JS_ToInt32(ctx, &length, lengthValue);

    return length;
}

int GUI_GetKey(JSContext *ctx, JSValue element)
{
    JSValue keyValue = JS_GetPropertyStr(ctx, element, "key");
    int key;
    JS_ToInt32(ctx, &key, keyValue);
    return key;
}

/// @brief Gets the children JS Object, if it exists, otherwise JS_UNDEFINED. Does not check if an actual element was given.
/// @param ctx
/// @param element
/// @return JS Object or JS_UNDEFINED
JSValue GUI_GetChildren(JSContext *ctx, JSValue element)
{
    if (!JS_IsObject(element))
    {
        fprintf(stderr, "[GUI_GetChildren] Element is not an object");
        exit(3);
        return JS_UNDEFINED;
    }
    JSValue props = JS_GetPropertyStr(ctx, element, "props");
    if (JS_IsUndefined(props))
        return JS_UNDEFINED;
    JSValue children = JS_GetPropertyStr(ctx, props, "children");
    if (JS_IsUndefined(props))
        return JS_UNDEFINED;

    return children;
}

static void renderChildren(JSContext *ctx, JSValue element)
{
    JSValue children = GUI_GetChildren(ctx, element);

    int length = GUI_GetLength(ctx, children);

    for (int i = 0; i < length; i++)
    {
        JSValue child = JS_GetPropertyUint32(ctx, children, i);
        GUI_RenderValue(ctx, child);
    }
}

void GUI_RenderCustom(JSContext *ctx, JSValue element)
{
    JSValue instance = JS_GetPropertyStr(ctx, element, "instance");
    JSValue render = JS_GetPropertyStr(ctx, instance, "render");
    if (!JS_IsFunction(ctx, render))
    {
        fprintf(stderr, "[GUI_RenderCustom] FATAL: Custom elements need a render() function.\n");
        exit(4);
        return;
    }
    JSValue argv[] = {JS_UNDEFINED};
    JSValue ret = JS_Call(ctx, render, instance, 0, argv);

    // GUI_RenderValue(ctx, JS_DupValue(ctx, ret));
}

void GUI_RenderStack(JSContext *ctx, JSValue element, char direction)
{
    // Determine layout direction.
    Clay_Sizing sizing;
    int dir;
    switch (direction)
    {
    case 'v':
        sizing = (Clay_Sizing){CLAY_SIZING_FIT(), CLAY_SIZING_GROW()};
        dir = CLAY_TOP_TO_BOTTOM;
        break;
    case 'h':
        sizing = (Clay_Sizing){CLAY_SIZING_GROW(), CLAY_SIZING_FIT()};
        dir = CLAY_LEFT_TO_RIGHT;
        break;
    default:
        fprintf(stderr, "[GUI_RenderStack] Stack direction '%c' unknown", direction);
        exit(1);
        return;
    }

    Clay_Padding padding = STYLES_GetPadding(ctx, element);
    Clay_Color backgroundColor = STYLES_GetBackgroundColor(ctx, element);
    CLAY((Clay_ElementDeclaration){
        .layout = {
            .layoutDirection = dir,
            .childAlignment = {
                CLAY_ALIGN_X_CENTER,
                CLAY_ALIGN_Y_CENTER,
            },
            .padding = padding,
            .sizing = sizing,
        },
        .backgroundColor = backgroundColor,
    })
    {
        EVENT_HandleMouseEvents(ctx, element);
        renderChildren(ctx, element);
    }
}

void GUI_RenderString(JSContext *ctx, JSValue element)
{

    JSValue stringElement = GUI_GetChildren(ctx, element);

    const char *string = JS_ToCString(ctx, stringElement);
    Clay_String clayString = {.chars = string, .length = strlen(string)};

    Clay_Color color = STYLES_GetColor(ctx, element);
    if (color.a + color.b + color.g + color.r <= 0)
    {
        color = (Clay_Color){0, 0, 0, 255};
    }

    // Set custom font size if set, otherwise default to 12.
    int fontSize = STYLES_GetFontSize(ctx, element);
    if (fontSize < 0)
        fontSize = 12;

    int letterSpacing = STYLES_GetLetterSpacing(ctx, element);
    if (letterSpacing < 0)
        letterSpacing = 1;

    Clay_Color backgroundColor = STYLES_GetBackgroundColor(ctx, element);
    CLAY((Clay_ElementDeclaration){
        .backgroundColor = backgroundColor,
    })
    {
        CLAY_TEXT(clayString, CLAY_TEXT_CONFIG((Clay_TextElementConfig){
                                  .textColor = color,
                                  .fontSize = fontSize,
                                  .letterSpacing = letterSpacing,
                              }));
    }
}

/// @brief Takes the prop from the passed element and applies it to all of its children (if the prop is not already set on these children)
/// @param ctx
/// @param element Parent element from which the prop should be copied
/// @param prop Name of the prop
void GUI_ApplyPropToChild(JSContext *ctx, JSValue element, char *prop)
{
    JSValue props = JS_GetPropertyStr(ctx, element, "props");
    JSValue givenProp = JS_GetPropertyStr(ctx, props, prop);
    if (!JS_IsUndefined(givenProp))
    {
        JSValue children = GUI_GetChildren(ctx, element);
        int length = GUI_GetLength(ctx, children);

        // Apply prop to all children
        for (int i = 0; i < length; i++)
        {
            JSValue child = JS_GetPropertyUint32(ctx, children, i);
            JSValue childProps = JS_GetPropertyStr(ctx, child, "props");

            // Never overwrite a prop on the child
            JSValue childProp = JS_GetPropertyStr(ctx, childProps, prop);
            if (JS_IsUndefined(childProp))
            {
                JS_SetPropertyStr(ctx, childProps, prop, givenProp);
            }
        }
    }
}

void GUI_RenderText(JSContext *ctx, JSValue element)
{
    GUI_ApplyPropToChild(ctx, element, "$color");
    GUI_ApplyPropToChild(ctx, element, "$fontSize");
    GUI_ApplyPropToChild(ctx, element, "$letterSpacing");

    Clay_Padding padding = STYLES_GetPadding(ctx, element);
    Clay_Color backgroundColor = STYLES_GetBackgroundColor(ctx, element);
    CLAY((Clay_ElementDeclaration){
        .backgroundColor = backgroundColor,
        .layout = {
            .padding = padding,
        },
    })
    {
        renderChildren(ctx, element);
    }
}

void GUI_RenderSpacer()
{
    CLAY((Clay_ElementDeclaration){
        .layout = {
            .sizing = {CLAY_SIZING_GROW(), CLAY_SIZING_GROW()},
        },
    })
    {
    }
}

/// @brief Handles rendering of the <group> component.
/// @note The group component only appends its props to children, so nothing is rendered.
/// @param node
void GUI_RenderGroup(JSContext *ctx, JSValue element)
{
    JSValue props = JS_GetPropertyStr(ctx, element, "props");
    JSValue children = JS_GetPropertyStr(ctx, props, "children");
    int num_children = GUI_GetLength(ctx, children);

    unsigned int num_keys;
    JSPropertyEnum *keys;
    int success = JS_GetOwnPropertyNames(ctx, &keys, &num_keys, props, JS_GPN_STRING_MASK | JS_GPN_ENUM_ONLY);
    if (success != 0)
    {
        fprintf(stderr, "[Error] Could not get property names\n");
        return;
    }

    // Apply all JSX properties to each descendant
    for (int j = 0; j < num_children; j++)
    {
        JSValue child = JS_GetPropertyUint32(ctx, children, j);
        JSValue childProps = JS_GetPropertyStr(ctx, child, "props");

        // Get all JSX properties
        for (int i = 0; i < num_keys; i++)
        {
            JSPropertyEnum key = keys[i];
            JSValue keyValue = JS_AtomToValue(ctx, key.atom);
            const char *keyStr = JS_ToCString(ctx, keyValue);

            // Never copy children
            if (0 == strcmp(keyStr, "children"))
                continue;

            // Only add property if child prop doesn't already have it.
            if (JS_HasProperty(ctx, childProps, key.atom))
                continue;

            JSValue groupPropValue = JS_GetPropertyStr(ctx, props, keyStr);
            success = JS_SetPropertyStr(ctx, childProps, keyStr, groupPropValue);
            if (success != 1)
            {
                printf("[Warning] could not set property in group to %s\n", keyStr);
            }
        }
    }
    JS_FreePropertyEnum(ctx, keys, num_keys);

    renderChildren(ctx, element);
}

void GUI_RenderArray(JSContext *ctx, JSValue element)
{
    int length = GUI_GetLength(ctx, element);
    for (int i = 0; i < length; i++)
    {
        JSValue child = JS_GetPropertyUint32(ctx, element, i);
        GUI_RenderValue(ctx, child);
    }
}

void GUI_RenderValue(JSContext *ctx, JSValue element)
{
    if (JS_IsUninitialized(element) || JS_IsUndefined(element))
        return;

    JSValue typeValue = JS_GetPropertyStr(ctx, element, "type");
    if (!JS_IsString(typeValue))
    {

        // Check if it could be an array.
        // JS_IsArray, somehow returns false, even if it is an array.
        int length = GUI_GetLength(ctx, element);
        if (JS_IsObject(element) && length > 0)
        {
            GUI_RenderArray(ctx, element);
            return;
        }

        fprintf(stderr, "[GUI_RenderValue] FATAL: element type is not a string\n");
        exit(2);
        return;
    }
    const char *type = JS_ToCString(ctx, typeValue);
    // printf("[GUI_RenderValue] Type is: %s\n", type);

    if (0 == strcmp(type, "custom"))
    {
        GUI_RenderCustom(ctx, element);
    }
    else if (0 == strcmp(type, "spacer"))
    {
        GUI_RenderSpacer();
    }
    else if (0 == strcmp(type, "hStack"))
    {
        GUI_RenderStack(ctx, element, 'h');
    }
    else if (0 == strcmp(type, "vStack"))
    {
        GUI_RenderStack(ctx, element, 'v');
    }
    else if (0 == strcmp(type, "text"))
    {
        GUI_RenderText(ctx, element);
    }
    else if (0 == strcmp(type, "string"))
    {
        GUI_RenderString(ctx, element);
    }
    else if (0 == strcmp(type, "group"))
    {
        GUI_RenderGroup(ctx, element);
    }
    else
    {
        fprintf(stderr, "unknown component: %s\n", type);
        exit(1);
    }

    JS_FreeCString(ctx, type);
}

Clay_RenderCommandArray GUI_RenderCommands(TJSRuntime *qrt)
{
    JSContext *ctx = TJS_GetJSContext(qrt);
    Clay_BeginLayout();

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
        GUI_RenderValue(ctx, rootValue);
    }

    return Clay_EndLayout();
}