#include "../lib/clay/clay.h"
#include "../lib/quickjs/quickjs.h"
#include "colors.h"
#include "styles.h"
#include "gui.h"

Clay_Color parseColor(char *colorStr)
{
    if ('#' == colorStr[0])
    {
        return parseHexColor(&colorStr[1]);
    }

    Clay_Color color = {0, 0, 0, 0};
    for (int i = 0; i < COLOR_Length; i++)
    {
        Clay_Color c = COLOR_Values[i];
        if (0 == strcmp(colorStr, COLOR_Names[i]))
        {
            color = COLOR_Values[i];
            break;
        }
    }

    return color;
}

Clay_Padding parsePadding(JSContext *ctx, JSValue propValue)
{
    Clay_Padding padding = {0};
    if (JS_IsNumber(propValue))
    {
        u_int32_t value = 0;
        JS_ToUint32(ctx, &value, propValue);
        padding = CLAY_PADDING_ALL(value);

        return padding;
    }

    if (!JS_IsObject(propValue))
    {
        fprintf(stderr, "[Warning] Padding property given but not as an object. Ignoring.");
        return padding;
    }

    JSValue placeVertical = JS_GetPropertyStr(ctx, propValue, "vertical");
    if (JS_IsNumber(placeVertical))
    {
        u_int32_t value = 0;
        JS_ToUint32(ctx, &value, placeVertical);
        padding.top = value;
        padding.bottom = value;
    }

    JSValue placeHorizontal = JS_GetPropertyStr(ctx, propValue, "horizontal");
    if (JS_IsNumber(placeHorizontal))
    {
        u_int32_t value = 0;
        JS_ToUint32(ctx, &value, placeHorizontal);
        padding.left = value;
        padding.right = value;
    }

    JSValue placeLeft = JS_GetPropertyStr(ctx, propValue, "left");
    if (JS_IsNumber(placeLeft))
    {
        int value = 0;
        JS_ToUint32(ctx, &value, placeLeft);
        padding.left = value;
    }

    JSValue placeRight = JS_GetPropertyStr(ctx, propValue, "right");
    if (JS_IsNumber(placeRight))
    {
        int value = 0;
        JS_ToUint32(ctx, &value, placeRight);
        padding.right = value;
    }

    JSValue placeTop = JS_GetPropertyStr(ctx, propValue, "top");
    if (JS_IsNumber(placeTop))
    {
        int value = 0;
        JS_ToUint32(ctx, &value, placeTop);
        padding.top = value;
    }

    JSValue placeBottom = JS_GetPropertyStr(ctx, propValue, "bottom");
    if (JS_IsNumber(placeBottom))
    {
        int value = 0;
        JS_ToUint32(ctx, &value, placeBottom);
        padding.bottom = value;
    }

    return padding;
}

DOMStyles *getAllStyles(struct DOMNode *node)
{
    // Get list of all properties of the current node
    int num_keys;
    JSPropertyEnum *keys;
    DOMStyles *styles = calloc(1, sizeof(DOMStyles));
    int success = JS_GetOwnPropertyNames(node->ctx, &keys, &num_keys, node->properties, JS_GPN_STRING_MASK | JS_GPN_ENUM_ONLY);
    if (success != 0)
    {
        fprintf(stderr, "[Error] Could not get property names for %s %d\n", node->type, node->key);
        return styles;
    }

    // Negative color means no color was set!
    styles->color = (Clay_Color){-1, -1, -1, -1};

    for (int i = 0; i < num_keys; i++)
    {
        JSPropertyEnum key = keys[i];
        char *keyStr = JS_AtomToCString(node->ctx, key.atom);
        JS_FreeCString(node->ctx, keyStr);

        JSValue propValue = JS_GetPropertyStr(node->ctx, node->properties, keyStr);

        if (0 == strcmp(keyStr, "$backgroundColor"))
        {
            char *str = JS_ToCString(node->ctx, propValue);
            styles->backgroundColor = parseColor(str);
            JS_FreeCString(node->ctx, str);
        }
        else if (0 == strcmp(keyStr, "$padding"))
        {
            styles->padding = parsePadding(node->ctx, propValue);
        }
        else if (0 == strcmp(keyStr, "$color"))
        {
            char *str = JS_ToCString(node->ctx, propValue);
            styles->color = parseColor(str);
            JS_FreeCString(node->ctx, str);
        }
        else if (0 == strcmp(keyStr, "$fontSize"))
        {
            int32_t fontSize;
            int status = JS_ToInt32(node->ctx, &fontSize, propValue);
            styles->fontSize = (uint16_t)fontSize;
        }
        else if (0 == strcmp(keyStr, "$letterSpacing"))
        {
            int32_t letterSpacing;
            int status = JS_ToInt32(node->ctx, &letterSpacing, propValue);
            // Don't allow negative values.
            if (letterSpacing < 0)
            {
                letterSpacing = 1;
            }
            styles->letterSpacing = (uint16_t)letterSpacing;
        }
    }
    JS_FreePropertyEnum(node->ctx, keys, num_keys);

    return styles;
}
