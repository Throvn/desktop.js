#include "../events/mouse.h"
#include "stdlib.h"
#include "draw.h"
#include "styles.h"
#include "../renderer/reconcile.h"
#include "../debug.h"
#include "./blob.h"

/// @brief Defined in js.c
extern JSValue rootValue;

void GUI_PrintKeys(JSContext *ctx, JSValueConst element)
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
        JS_FreeValue(ctx, key_val);

        printf("Key[%u]: %s\n", i, key_str);
        JS_FreeCString(ctx, key_str);
        JS_FreeAtom(ctx, props[i].atom);
    }

    JS_FreePropertyEnum(ctx, props, len);
}

/// Checks if the JSValue could be an element by checking, if
/// the JSValue includes all necessary parameters and their expected types.
bool GUI_IsElement(JSContext *ctx, JSValueConst element)
{
    if (!JS_IsObject(element))
        return false;
    JSValue key = JS_GetPropertyStr(ctx, element, "key");
    if (!JS_IsNumber(key))
    {
        JS_FreeValue(ctx, key);
        return false;
    }
    JSValue props = JS_GetPropertyStr(ctx, element, "props");
    if (!JS_IsArray(props))
    {
        JS_FreeValue(ctx, key);
        JS_FreeValue(ctx, props);
        return false;
    }
    JSValue type = JS_GetPropertyStr(ctx, element, "type");
    bool isElement = JS_IsString(type);

    JS_FreeValue(ctx, key);
    JS_FreeValue(ctx, props);
    JS_FreeValue(ctx, type);
    return isElement;
}

/// @brief Gives back the value of the length property of the passed element.
int GUI_GetLength(JSContext *ctx, JSValueConst element)
{
    if (!JS_IsObject(element))
    {
        return -1;
    }

    JSValue lengthValue = JS_GetPropertyStr(ctx, element, "length");
    if (JS_IsUndefined(lengthValue))
    {
        GUI_PrintKeys(ctx, element);
        JS_FreeValue(ctx, lengthValue);
        exit(1);
    }
    int length;
    JS_ToInt32(ctx, &length, lengthValue);

    JS_FreeValue(ctx, lengthValue);
    return length;
}

int GUI_GetKey(JSContext *ctx, JSValueConst element)
{
    JSValue keyValue = JS_GetPropertyStr(ctx, element, "key");
    int key;
    JS_ToInt32(ctx, &key, keyValue);

    JS_FreeValue(ctx, keyValue);
    return key;
}

/// @brief Gets the children JS Object, if it exists, otherwise JS_UNDEFINED. Does not check if an actual element was given.
/// @param ctx
/// @param element
/// @return JS Object or JS_UNDEFINED
JSValue GUI_GetChildren(JSContext *ctx, JSValueConst element)
{
    if (!JS_IsObject(element))
    {
        fprintf(stderr, "[GUI_GetChildren] Element is not an object");
        exit(3);
        return JS_UNDEFINED;
    }
    JSValue props = JS_GetPropertyStr(ctx, element, "props");
    if (JS_IsUndefined(props))
    {
        JS_FreeValue(ctx, props);
        return JS_UNDEFINED;
    }
    JSValue children = JS_GetPropertyStr(ctx, props, "children");

    JS_FreeValue(ctx, props);
    return children;
}

static void renderChildren(JSContext *ctx, JSValueConst element)
{
    JSValue children = GUI_GetChildren(ctx, element);

    int length = GUI_GetLength(ctx, children);

    for (int i = 0; i < length; i++)
    {
        JSValue child = JS_GetPropertyUint32(ctx, children, i);
        GUI_RenderValue(ctx, child);
        JS_FreeValue(ctx, child);
    }
    JS_FreeValue(ctx, children);
}

void GUI_RenderCustom(JSContext *ctx, JSValueConst element)
{
    JSValue instance = JS_GetPropertyStr(ctx, element, "instance");
    // Create a new instance only if it doesn't exist yet.
    if (JS_IsUndefined(instance))
    {
        JS_FreeValue(ctx, instance);

        JSValue class = JS_GetPropertyStr(ctx, element, "class");
        JSValue props = JS_GetPropertyStr(ctx, element, "props");
        JSValue ret = JS_CallConstructor(ctx, class, 1, &props);
        if (JS_IsException(ret))
        {
            TJSRuntime *qrt = TJS_GetRuntime(ctx);
            TJS_Stop(qrt);
            return;
        }

        JS_FreeValue(ctx, class);
        JS_FreeValue(ctx, props);

        JS_SetPropertyStr(ctx, element, "instance", JS_DupValue(ctx, ret));
        instance = ret;
    }

    JSValue render = JS_GetPropertyStr(ctx, instance, "render");
    if (!JS_IsFunction(ctx, render))
    {
        fprintf(stderr, "[GUI_RenderCustom] FATAL: Custom elements need a render() function.\n");
        const char *functionName = JS_ToCString(ctx, render);
        fprintf(stderr, "[GUI_RenderCustom] FATAL: However, render() function is: %s\n", functionName);
        JS_FreeCString(ctx, functionName);
        JS_FreeValue(ctx, instance);
        JS_FreeValue(ctx, render);
        exit(4);
        return;
    }
    JSValue ret = JS_Call(ctx, render, instance, 0, NULL);

    JSValue renderChild = JS_GetPropertyStr(ctx, element, "_renderChild");
    GUI_Diff(ctx, renderChild, ret);

    GUI_RenderValue(ctx, ret);
    JS_SetPropertyStr(ctx, element, "_renderChild", ret);

    JS_FreeValue(ctx, instance);
    JS_FreeValue(ctx, render);
    JS_FreeValue(ctx, renderChild);
}

void GUI_RenderStack(JSContext *ctx, JSValue element, char direction)
{
    // Determine layout direction.
    Clay_Sizing sizing;
    int dir;
    switch (direction)
    {
    case 'v':
        sizing = (Clay_Sizing){CLAY_SIZING_GROW(), CLAY_SIZING_GROW()};
        dir = CLAY_TOP_TO_BOTTOM;
        break;
    case 'h':
        sizing = (Clay_Sizing){CLAY_SIZING_GROW(), CLAY_SIZING_GROW()};
        dir = CLAY_LEFT_TO_RIGHT;
        break;
    default:
        fprintf(stderr, "[GUI_RenderStack] Stack direction '%c' unknown", direction);
        exit(1);
        return;
    }

    Clay_Padding padding = STYLES_GetPadding(ctx, element);
    Clay_Color backgroundColor = STYLES_GetBackgroundColor(ctx, element);
    uint16_t childGap = STYLES_GetGap(ctx, element);
    Clay_CornerRadius cornerRadius = STYLES_GetBorderRadius(ctx, element);

    CLAY((Clay_ElementDeclaration){
        .layout = {
            .layoutDirection = dir,
            .childAlignment = {
                CLAY_ALIGN_X_CENTER,
                CLAY_ALIGN_Y_CENTER,
            },
            .padding = padding,
            .sizing = sizing,
            .childGap = childGap,
        },
        .backgroundColor = backgroundColor,
        .cornerRadius = cornerRadius,
    })
    {
        EVENT_HandleMouseEvents(ctx, element);
        renderChildren(ctx, element);
    }
}

void GUI_RenderImage(JSContext *ctx, JSValueConst element)
{
    JSValue props = JS_GetPropertyStr(ctx, element, "props");
    JSValue data = JS_GetPropertyStr(ctx, props, "data");
    if (!JS_IsObject(data))
    {
        JS_FreeValue(ctx, data);
        JS_FreeValue(ctx, props);
        return;
    }

    JS_FreeValue(ctx, props);

    int size = JS_GetBlobSize(ctx, data);
    uint8_t *imageData = malloc(size);
    int status = JS_GetBlobUint8Array(ctx, data, imageData);
    if (status < 0)
    {
        JS_FreeValue(ctx, data);
        free(imageData);
        return;
    }

    JSValue typeValue = JS_GetPropertyStr(ctx, data, "type");
    const char *type = JS_ToCString(ctx, typeValue);
    JS_FreeValue(ctx, typeValue);
    JS_FreeValue(ctx, data);
    if (strncmp(type, "image/", 6))
    {
        JS_FreeCString(ctx, type);
        free(imageData);
        return;
    }

    char *imageExt = calloc(1, strlen(type) - 4);
    imageExt[0] = '.';
    strcpy(&imageExt[1], &type[6]);
    JS_FreeCString(ctx, type);

    Image *img = malloc(sizeof(Image));
    *img = LoadImageFromMemory(imageExt, imageData, size);

    free(imageExt);
    free(imageData);

    Texture2D *texture = tex_alloc();
    *texture = LoadTextureFromImage(*img);

    UnloadImage(*img);
    free(img);

    CLAY((Clay_ElementDeclaration){
        .image = {
            .imageData = texture,

        },
        .layout = {
            .sizing = {
                .height = 200,
                .width = 300,
            },
        },
    })
    {
    }
}

void GUI_RenderString(JSContext *ctx, JSValueConst element)
{

    JSValue stringElement = GUI_GetChildren(ctx, element);

    const char *string = JS_ToCString(ctx, stringElement);
    JS_FreeValue(ctx, stringElement);

    char *str = a_alloc(strlen(string) + 1);
    strcpy(str, string);
    JS_FreeCString(ctx, string);

    Clay_String clayString = {.chars = str, .length = strlen(str)};

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

    int lineHeight = STYLES_GetLineHeight(ctx, element);
    if (lineHeight == -1)
        lineHeight = fontSize;

    Clay_Color backgroundColor = STYLES_GetBackgroundColor(ctx, element);
    CLAY((Clay_ElementDeclaration){
        .backgroundColor = backgroundColor,
    })
    {
        CLAY_TEXT(clayString, CLAY_TEXT_CONFIG((Clay_TextElementConfig){
                                  .textColor = color,
                                  .fontSize = fontSize,
                                  .letterSpacing = letterSpacing,
                                  .lineHeight = lineHeight,
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
    JS_FreeValue(ctx, props);

    if (JS_IsUndefined(givenProp))
    {
        JS_FreeValue(ctx, givenProp);
        return;
    }

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
            JS_SetPropertyStr(ctx, childProps, prop, JS_DupValue(ctx, givenProp));
        }

        JS_FreeValue(ctx, child);
        JS_FreeValue(ctx, childProps);
        JS_FreeValue(ctx, childProp);
    }

    JS_FreeValue(ctx, givenProp);
    JS_FreeValue(ctx, children);
}

void GUI_RenderText(JSContext *ctx, JSValue element)
{
    GUI_ApplyPropToChild(ctx, element, "$color");
    GUI_ApplyPropToChild(ctx, element, "$fontSize");
    GUI_ApplyPropToChild(ctx, element, "$letterSpacing");
    GUI_ApplyPropToChild(ctx, element, "$lineHeight");

    Clay_Padding padding = STYLES_GetPadding(ctx, element);
    Clay_Color backgroundColor = STYLES_GetBackgroundColor(ctx, element);
    Clay_CornerRadius cornerRadius = STYLES_GetBorderRadius(ctx, element);
    CLAY((Clay_ElementDeclaration){
        .backgroundColor = backgroundColor,
        .layout = {
            .padding = padding,
        },
        .cornerRadius = cornerRadius,
    })
    {
        EVENT_HandleMouseEvents(ctx, element);
        renderChildren(ctx, element);
    }
}

void GUI_RenderSpacer()
{
    CLAY((Clay_ElementDeclaration){
        .layout = {
            .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)},
        },
    })
    {
    }
}

/// @brief Handles rendering of the <group> component.
/// @note The group component only appends its props to children, so nothing is rendered.
/// @param node
void GUI_RenderGroup(JSContext *ctx, JSValueConst element)
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
        JS_FreeValue(ctx, props);
        JS_FreeValue(ctx, children);
        return;
    }

    // Apply all JSX properties to each descendant
    for (int j = 0; j < num_children; j++)
    {
        JSValue child = JS_GetPropertyUint32(ctx, children, j);
        JSValue childProps = JS_GetPropertyStr(ctx, child, "props");
        JS_FreeValue(ctx, child);

        // Get all JSX properties
        for (int i = 0; i < num_keys; i++)
        {
            JSPropertyEnum key = keys[i];
            JSValue keyValue = JS_AtomToValue(ctx, key.atom);
            const char *keyStr = JS_ToCString(ctx, keyValue);
            JS_FreeValue(ctx, keyValue);

            // Never copy children
            if (0 == strcmp(keyStr, "children"))
            {
                JS_FreeCString(ctx, keyStr);
                continue;
            }

            // Only add property if child prop doesn't already have it.
            if (true == JS_HasProperty(ctx, childProps, key.atom))
            {
                JS_FreeCString(ctx, keyStr);
                continue;
            }

            JSValue groupPropValue = JS_GetPropertyStr(ctx, props, keyStr);
            JS_SetPropertyStr(ctx, childProps, keyStr, groupPropValue);

            JS_FreeCString(ctx, keyStr);
        }

        JS_FreeValue(ctx, childProps);
    }
    JS_FreePropertyEnum(ctx, keys, num_keys);
    JS_FreeValue(ctx, props);
    JS_FreeValue(ctx, children);

    renderChildren(ctx, element);
}

void GUI_RenderArray(JSContext *ctx, JSValueConst element)
{
    int length = GUI_GetLength(ctx, element);
    for (int i = 0; i < length; i++)
    {
        JSValue child = JS_GetPropertyUint32(ctx, element, i);
        GUI_RenderValue(ctx, child);
        JS_FreeValue(ctx, child);
    }
}

void GUI_RenderValue(JSContext *ctx, JSValue element)
{
    if (JS_IsUninitialized(element) || JS_IsUndefined(element) || JS_IsNull(element))
    {
        JS_FreeValue(ctx, element);
        return;
    }

    if (JS_IsException(element))
    {
        TJSRuntime *qrt = TJS_GetRuntime(ctx);
        TJS_Stop(qrt);
        return;
    }

    JSValue typeValue = JS_GetPropertyStr(ctx, element, "type");
    if (!JS_IsString(typeValue))
    {

        // Check if it could be an array.
        // JS_IsArray, somehow returns false, even if it is an array.
        int length = GUI_GetLength(ctx, element);
        if (JS_IsObject(element) && length > 0)
        {
            GUI_RenderArray(ctx, element);
            JS_FreeValue(ctx, typeValue);
            return;
        }

        fprintf(stderr, "[GUI_RenderValue] FATAL: element type is not a string\n");
        const char *elementString = JS_ToCString(ctx, element);
        fprintf(stderr, "[GUI_RenderValue] FATAL: element: %s %d\n", elementString, JS_VALUE_GET_TAG(element));
        JS_FreeCString(ctx, elementString);
        JS_FreeValue(ctx, typeValue);
        exit(2);
        return;
    }
    const char *type = JS_ToCString(ctx, typeValue);
    JS_FreeValue(ctx, typeValue);

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
    else if (0 == strcmp(type, "image"))
    {
        GUI_RenderImage(ctx, element);
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
            .sizing = {CLAY_SIZING_GROW(), CLAY_SIZING_GROW()},
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