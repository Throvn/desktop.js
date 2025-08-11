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

JSValue GUI_GetChildren(JSContext *ctx, JSValue element)
{
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
    JSValue ret = JS_Call(ctx, render, instance, 0, NULL);

    GUI_RenderValue(ctx, ret);
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

    JSValue keyValue = JS_GetPropertyStr(ctx, element, "key");
    int key;
    JS_ToInt64(ctx, &key, keyValue);

    CLAY((Clay_ElementDeclaration){
        .id = key,
        .layout = {
            .layoutDirection = dir,
            .childAlignment = {
                CLAY_ALIGN_X_CENTER,
                CLAY_ALIGN_Y_CENTER,
            },
        },
    })
    {
        renderChildren(ctx, element);
    }
}

void GUI_RenderString(JSContext *ctx, JSValue element)
{

    char *string = JS_ToCString(ctx, element);
    Clay_String clayString = {.chars = string, .length = strlen(string)};
    CLAY_TEXT(clayString, CLAY_TEXT_CONFIG((Clay_TextElementConfig){
                              .fontSize = 12,
                              .textColor = (Clay_Color){0, 0, 0, 255},
                              .letterSpacing = 5,
                          }));
}

void GUI_RenderText(JSContext *ctx, JSValue element)
{
    JSValue children = GUI_GetChildren(ctx, element);
    int length = GUI_GetLength(ctx, children);

    // TODO: apply text styles to each child.

    int key = GUI_GetKey(ctx, element);
    Clay_Padding padding = GUI_GetPadding(ctx, element);
    CLAY((Clay_ElementDeclaration){
        .id = key,
        // .backgroundColor = styles->backgroundColor,
        .layout = {
            .padding = padding,
        }})
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

void GUI_RenderValue(JSContext *ctx, JSValue element)
{
    if (JS_IsUninitialized(element))
        return;

    // Handle plain strings.
    if (JS_IsString(element) || JS_IsNumber(element))
    {
        GUI_RenderString(ctx, element);
        return;
    }

    JSValue typeValue = JS_GetPropertyStr(ctx, element, "type");
    if (!JS_IsString(typeValue))
    {
        fprintf(stderr, "[GUI_RenderValue] FATAL: element type is not a string\n");
        exit(2);
        return;
    }

    char *type = JS_ToCString(ctx, typeValue);
    // printf("[GUI_RenderValue] Type is: %s\n", type);

    if (0 == strcmp(type, "custom"))
    {
        GUI_RenderCustom(ctx, element);
    }
    else if (0 == strcmp(type, "spacer"))
    {
        GUI_RenderSpacer(ctx, element);
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
        .layout = {
            .sizing = {
                CLAY_SIZING_GROW(),
                CLAY_SIZING_GROW(),
            },
            .childAlignment = {
                .x = CLAY_ALIGN_X_CENTER,
                .y = CLAY_ALIGN_Y_CENTER,
            }},
        .backgroundColor = (Clay_Color){255, 255, 255, 255}})
    {
        GUI_RenderValue(ctx, rootValue);
    }

    return Clay_EndLayout();
}