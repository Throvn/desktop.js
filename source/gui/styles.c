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
        // fprintf(stderr, "[Warning] Padding property given but not as an object. Ignoring.\n");
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
        uint32_t value = 0;
        JS_ToUint32(ctx, &value, placeLeft);
        padding.left = value;
    }

    JSValue placeRight = JS_GetPropertyStr(ctx, propValue, "right");
    if (JS_IsNumber(placeRight))
    {
        uint32_t value = 0;
        JS_ToUint32(ctx, &value, placeRight);
        padding.right = value;
    }

    JSValue placeTop = JS_GetPropertyStr(ctx, propValue, "top");
    if (JS_IsNumber(placeTop))
    {
        uint32_t value = 0;
        JS_ToUint32(ctx, &value, placeTop);
        padding.top = value;
    }

    JSValue placeBottom = JS_GetPropertyStr(ctx, propValue, "bottom");
    if (JS_IsNumber(placeBottom))
    {
        uint32_t value = 0;
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

Clay_Color parseColor(const char *colorStr)
{
    if ('#' == colorStr[0])
    {
        return parseHexColor(&colorStr[1]);
    }

    Clay_Color color = {0, 0, 0, 0};
    for (int i = 0; i < COLOR_Length; i++)
    {
        if (0 == strcmp(colorStr, COLOR_Names[i]))
        {
            color = COLOR_Values[i];
            break;
        }
    }

    return color;
}

static Clay_Color getColorFromProperty(JSContext *ctx, JSValue element, char *propName)
{
    JSValue props = JS_GetPropertyStr(ctx, element, "props");
    if (!JS_IsObject(props))
    {
        return (Clay_Color){0};
    }

    JSValue colorValue = JS_GetPropertyStr(ctx, props, propName);
    if (!JS_IsString(colorValue))
    {
        return (Clay_Color){0};
    }

    const char *colorStr = JS_ToCString(ctx, colorValue);
    Clay_Color color = parseColor(colorStr);
    JS_FreeCString(ctx, colorStr);

    return color;
}

Clay_Color STYLES_GetBackgroundColor(JSContext *ctx, JSValue element)
{
    return getColorFromProperty(ctx, element, "$backgroundColor");
}

Clay_Color STYLES_GetColor(JSContext *ctx, JSValue element)
{
    return getColorFromProperty(ctx, element, "$color");
}

int STYLES_GetPropValueAsInt32(JSContext *ctx, JSValue element, char *prop)
{

    JSValue props = JS_GetPropertyStr(ctx, element, "props");
    if (JS_IsUndefined(props))
        return -1;

    JSValue propValue = JS_GetPropertyStr(ctx, props, prop);
    if (!JS_IsNumber(propValue))
        return -1;

    int fontSize;
    JS_ToInt32(ctx, &fontSize, propValue);
    return fontSize;
}

int STYLES_GetFontSize(JSContext *ctx, JSValue element)
{
    return STYLES_GetPropValueAsInt32(ctx, element, "$fontSize");
}

int STYLES_GetLetterSpacing(JSContext *ctx, JSValue element)
{
    return STYLES_GetPropValueAsInt32(ctx, element, "$letterSpacing");
}

uint16_t STYLES_GetGap(JSContext *ctx, JSValue element)
{
    JSValue props = JS_GetPropertyStr(ctx, element, "props");
    if (!JS_IsObject(props))
        return 0;

    JSValue gapValue = JS_GetPropertyStr(ctx, props, "$gap");
    if (!JS_IsNumber(gapValue))
        return 0;

    uint16_t gap;
    int failed = JS_ToUint32(ctx, &gap, gapValue);
    if (failed)
        return 0;

    return gap;
}

static Clay_CornerRadius parseCornerRadius(JSContext *ctx, JSValue propValue)
{
    Clay_CornerRadius cornerRadius = {0};
    if (JS_IsNumber(propValue))
    {
        double value = 0;
        JS_ToFloat64(ctx, &value, propValue);
        cornerRadius = CLAY_CORNER_RADIUS((float)value);

        return cornerRadius;
    }

    if (!JS_IsObject(propValue))
    {
        // fprintf(stderr, "[Warning] Padding property given but not as an object. Ignoring.\n");
        return cornerRadius;
    }

    JSValue placeTop = JS_GetPropertyStr(ctx, propValue, "top");
    if (JS_IsNumber(placeTop))
    {
        double value = 0;
        JS_ToFloat64(ctx, &value, placeTop);
        cornerRadius.topLeft = (float)value;
        cornerRadius.topRight = (float)value;
    }

    JSValue placeBottom = JS_GetPropertyStr(ctx, propValue, "bottom");
    if (JS_IsNumber(placeBottom))
    {
        double value = 0;
        JS_ToFloat64(ctx, &value, placeBottom);
        cornerRadius.bottomLeft = (float)value;
        cornerRadius.bottomRight = (float)value;
    }

    JSValue placeLeft = JS_GetPropertyStr(ctx, propValue, "left");
    if (JS_IsNumber(placeLeft))
    {
        double value = 0;
        JS_ToFloat64(ctx, &value, placeLeft);
        cornerRadius.topLeft = (float)value;
        cornerRadius.bottomLeft = (float)value;
    }

    JSValue placeRight = JS_GetPropertyStr(ctx, propValue, "right");
    if (JS_IsNumber(placeRight))
    {
        double value = 0;
        JS_ToFloat64(ctx, &value, placeRight);
        cornerRadius.topRight = (float)value;
        cornerRadius.bottomRight = (float)value;
    }

    JSValue placeTopLeft = JS_GetPropertyStr(ctx, propValue, "topLeft");
    if (JS_IsNumber(placeTopLeft))
    {
        double value = 0;
        JS_ToFloat64(ctx, &value, placeTopLeft);
        cornerRadius.topLeft = (float)value;
    }

    JSValue placeTopRight = JS_GetPropertyStr(ctx, propValue, "topRight");
    if (JS_IsNumber(placeTopRight))
    {
        double value = 0;
        JS_ToFloat64(ctx, &value, placeTopRight);
        cornerRadius.topRight = (float)value;
    }

    JSValue placeBottomLeft = JS_GetPropertyStr(ctx, propValue, "bottomLeft");
    if (JS_IsNumber(placeBottomLeft))
    {
        double value = 0;
        JS_ToFloat64(ctx, &value, placeBottomLeft);
        cornerRadius.bottomLeft = (float)value;
    }

    JSValue placeBottomRight = JS_GetPropertyStr(ctx, propValue, "bottomRight");
    if (JS_IsNumber(placeBottomRight))
    {
        double value = 0;
        JS_ToFloat64(ctx, &value, placeBottomRight);
        cornerRadius.bottomRight = (float)value;
    }

    return cornerRadius;
}

Clay_CornerRadius STYLES_GetBorderRadius(JSContext *ctx, JSValue element)
{
    JSValue props = JS_GetPropertyStr(ctx, element, "props");
    if (!JS_IsObject(props))
    {
        return (Clay_CornerRadius){0};
    }
    JSValue borderRadiusValue = JS_GetPropertyStr(ctx, props, "$borderRadius");
    Clay_CornerRadius extractedValues = parseCornerRadius(ctx, borderRadiusValue);
    return extractedValues;
}