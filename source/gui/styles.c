#include "colors.h"
#include "styles.h"

static Clay_Padding parsePadding(JSContext *ctx, JSValueConst propValue)
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
    JS_FreeValue(ctx, placeVertical);

    JSValue placeHorizontal = JS_GetPropertyStr(ctx, propValue, "horizontal");
    if (JS_IsNumber(placeHorizontal))
    {
        u_int32_t value = 0;
        JS_ToUint32(ctx, &value, placeHorizontal);
        padding.left = value;
        padding.right = value;
    }
    JS_FreeValue(ctx, placeHorizontal);

    JSValue placeLeft = JS_GetPropertyStr(ctx, propValue, "left");
    if (JS_IsNumber(placeLeft))
    {
        uint32_t value = 0;
        JS_ToUint32(ctx, &value, placeLeft);
        padding.left = value;
    }
    JS_FreeValue(ctx, placeLeft);

    JSValue placeRight = JS_GetPropertyStr(ctx, propValue, "right");
    if (JS_IsNumber(placeRight))
    {
        uint32_t value = 0;
        JS_ToUint32(ctx, &value, placeRight);
        padding.right = value;
    }
    JS_FreeValue(ctx, placeRight);

    JSValue placeTop = JS_GetPropertyStr(ctx, propValue, "top");
    if (JS_IsNumber(placeTop))
    {
        uint32_t value = 0;
        JS_ToUint32(ctx, &value, placeTop);
        padding.top = value;
    }
    JS_FreeValue(ctx, placeTop);

    JSValue placeBottom = JS_GetPropertyStr(ctx, propValue, "bottom");
    if (JS_IsNumber(placeBottom))
    {
        uint32_t value = 0;
        JS_ToUint32(ctx, &value, placeBottom);
        padding.bottom = value;
    }
    JS_FreeValue(ctx, placeBottom);

    return padding;
}

Clay_Padding STYLES_GetPadding(JSContext *ctx, JSValueConst element)
{

    JSValue props = JS_GetPropertyStr(ctx, element, "props");
    if (!JS_IsObject(props))
    {
        fprintf(stderr, "[GUI_GetPadding] Passed element does not have 'props' property");
        JS_FreeValue(ctx, props);
        return (Clay_Padding){0};
    }
    JSValue paddingValue = JS_GetPropertyStr(ctx, props, "$padding");
    JS_FreeValue(ctx, props);
    Clay_Padding padding = parsePadding(ctx, paddingValue);

    JS_FreeValue(ctx, paddingValue);
    return padding;
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

static Clay_Color getColorFromProperty(JSContext *ctx, JSValueConst element, const char *propName)
{
    JSValue props = JS_GetPropertyStr(ctx, element, "props");
    if (!JS_IsObject(props))
    {
        JS_FreeValue(ctx, props);
        return (Clay_Color){0};
    }

    JSValue colorValue = JS_GetPropertyStr(ctx, props, propName);
    JS_FreeValue(ctx, props);
    if (!JS_IsString(colorValue))
    {
        JS_FreeValue(ctx, colorValue);
        return (Clay_Color){0};
    }

    const char *colorStr = JS_ToCString(ctx, colorValue);
    Clay_Color color = parseColor(colorStr);

    JS_FreeCString(ctx, colorStr);
    JS_FreeValue(ctx, colorValue);
    return color;
}

Clay_Color STYLES_GetBackgroundColor(JSContext *ctx, JSValueConst element)
{
    return getColorFromProperty(ctx, element, "$backgroundColor");
}

Clay_Color STYLES_GetColor(JSContext *ctx, JSValueConst element)
{
    return getColorFromProperty(ctx, element, "$color");
}

int STYLES_GetPropValueAsInt32(JSContext *ctx, JSValueConst element, const char *prop)
{

    JSValue props = JS_GetPropertyStr(ctx, element, "props");
    if (!JS_IsObject(props))
    {
        JS_FreeValue(ctx, props);
        return -1;
    }

    JSValue propValue = JS_GetPropertyStr(ctx, props, prop);
    JS_FreeValue(ctx, props);
    if (!JS_IsNumber(propValue))
    {
        JS_FreeValue(ctx, propValue);
        return -1;
    }

    int fontSize;
    JS_ToInt32(ctx, &fontSize, propValue);

    JS_FreeValue(ctx, propValue);

    return fontSize;
}

int STYLES_GetFontSize(JSContext *ctx, JSValueConst element)
{
    return STYLES_GetPropValueAsInt32(ctx, element, "$fontSize");
}

int STYLES_GetLetterSpacing(JSContext *ctx, JSValueConst element)
{
    return STYLES_GetPropValueAsInt32(ctx, element, "$letterSpacing");
}

uint16_t STYLES_GetGap(JSContext *ctx, JSValueConst element)
{
    JSValue props = JS_GetPropertyStr(ctx, element, "props");
    if (!JS_IsObject(props))
    {
        JS_FreeValue(ctx, props);
        return 0;
    }

    JSValue gapValue = JS_GetPropertyStr(ctx, props, "$gap");
    JS_FreeValue(ctx, props);
    if (!JS_IsNumber(gapValue))
    {
        JS_FreeValue(ctx, gapValue);
        return 0;
    }

    uint16_t gap;
    int failed = JS_ToUint32(ctx, (uint32_t *)&gap, gapValue);
    JS_FreeValue(ctx, gapValue);
    if (failed)
        return 0;

    return gap;
}

static Clay_CornerRadius parseCornerRadius(JSContext *ctx, JSValueConst propValue)
{
    Clay_CornerRadius cornerRadius = {0, 0, 0, 0};
    if (JS_IsNumber(propValue))
    {
        double value = 0;
        JS_ToFloat64(ctx, &value, propValue);
        cornerRadius = CLAY_CORNER_RADIUS((float)value);

        return cornerRadius;
    }

    if (!JS_IsObject(propValue))
        return cornerRadius;

    JSValue placeTop = JS_GetPropertyStr(ctx, propValue, "top");
    if (JS_IsNumber(placeTop))
    {
        double value = 0;
        JS_ToFloat64(ctx, &value, placeTop);
        cornerRadius.topLeft = (float)value;
        cornerRadius.topRight = (float)value;
    }
    JS_FreeValue(ctx, placeTop);

    JSValue placeBottom = JS_GetPropertyStr(ctx, propValue, "bottom");
    if (JS_IsNumber(placeBottom))
    {
        double value = 0;
        JS_ToFloat64(ctx, &value, placeBottom);
        cornerRadius.bottomLeft = (float)value;
        cornerRadius.bottomRight = (float)value;
    }
    JS_FreeValue(ctx, placeBottom);

    JSValue placeLeft = JS_GetPropertyStr(ctx, propValue, "left");
    if (JS_IsNumber(placeLeft))
    {
        double value = 0;
        JS_ToFloat64(ctx, &value, placeLeft);
        cornerRadius.topLeft = (float)value;
        cornerRadius.bottomLeft = (float)value;
    }
    JS_FreeValue(ctx, placeLeft);

    JSValue placeRight = JS_GetPropertyStr(ctx, propValue, "right");
    if (JS_IsNumber(placeRight))
    {
        double value = 0;
        JS_ToFloat64(ctx, &value, placeRight);
        cornerRadius.topRight = (float)value;
        cornerRadius.bottomRight = (float)value;
    }
    JS_FreeValue(ctx, placeRight);

    JSValue placeTopLeft = JS_GetPropertyStr(ctx, propValue, "topLeft");
    if (JS_IsNumber(placeTopLeft))
    {
        double value = 0;
        JS_ToFloat64(ctx, &value, placeTopLeft);
        cornerRadius.topLeft = (float)value;
    }
    JS_FreeValue(ctx, placeTopLeft);

    JSValue placeTopRight = JS_GetPropertyStr(ctx, propValue, "topRight");
    if (JS_IsNumber(placeTopRight))
    {
        double value = 0;
        JS_ToFloat64(ctx, &value, placeTopRight);
        cornerRadius.topRight = (float)value;
    }
    JS_FreeValue(ctx, placeTopRight);

    JSValue placeBottomLeft = JS_GetPropertyStr(ctx, propValue, "bottomLeft");
    if (JS_IsNumber(placeBottomLeft))
    {
        double value = 0;
        JS_ToFloat64(ctx, &value, placeBottomLeft);
        cornerRadius.bottomLeft = (float)value;
    }
    JS_FreeValue(ctx, placeBottomLeft);

    JSValue placeBottomRight = JS_GetPropertyStr(ctx, propValue, "bottomRight");
    if (JS_IsNumber(placeBottomRight))
    {
        double value = 0;
        JS_ToFloat64(ctx, &value, placeBottomRight);
        cornerRadius.bottomRight = (float)value;
    }
    JS_FreeValue(ctx, placeBottomRight);

    return cornerRadius;
}

Clay_CornerRadius STYLES_GetBorderRadius(JSContext *ctx, JSValueConst element)
{
    JSValue props = JS_GetPropertyStr(ctx, element, "props");
    if (!JS_IsObject(props))
    {
        JS_FreeValue(ctx, props);
        return (Clay_CornerRadius){0};
    }
    JSValue borderRadiusValue = JS_GetPropertyStr(ctx, props, "$borderRadius");
    Clay_CornerRadius extractedValues = parseCornerRadius(ctx, borderRadiusValue);

    JS_FreeValue(ctx, props);
    JS_FreeValue(ctx, borderRadiusValue);
    return extractedValues;
}

int STYLES_GetLineHeight(JSContext *ctx, JSValueConst element)
{
    return STYLES_GetPropValueAsInt32(ctx, element, "$lineHeight");
}