#include "colors.h"
#include "styles.h"

static Clay_Padding parsePadding(JSContext *ctx, JSValue propValue)
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
        fprintf(stderr, "[Warning] Padding property given but not as an object. Ignoring.\n");
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

Clay_Padding STYLES_GetPadding(JSContext *ctx, JSValue element)
{

    JSValue props = JS_GetPropertyStr(ctx, element, "props");
    if (!JS_IsObject(props))
    {
        fprintf(stderr, "[GUI_GetPadding] Passed element does not have 'props' property");
        return (Clay_Padding){0};
    }
    JSValue paddingValue = JS_GetPropertyStr(ctx, props, "$padding");
    return parsePadding(ctx, paddingValue);
}

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

Clay_Color STYLES_GetBackgroundColor(JSContext *ctx, JSValue element)
{
    JSValue props = JS_GetPropertyStr(ctx, element, "props");
    if (!JS_IsObject(props))
    {
        return (Clay_Color){0, 0, 0, 0};
    }

    JSValue colorValue = JS_GetPropertyStr(ctx, props, "$backgroundColor");
    if (!JS_IsString(colorValue))
    {
        return (Clay_Color){0, 0, 0, 0};
    }

    char *colorStr = JS_ToCString(ctx, colorValue);
    Clay_Color color = parseColor(colorStr);
    JS_FreeCString(ctx, colorStr);

    return color;
}
