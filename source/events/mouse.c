#include "mouse.h"
#include <stdlib.h>

extern JSValue rootValue;

JSValue createMouseEvent(JSContext *ctx)
{
    JSValue mouseEvent = JS_NewObject(ctx);

    // Add altKey property
    int isAltKeyPressed = IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT);
    JSValue isAltKeyPressedJSValue = JS_NewBool(ctx, isAltKeyPressed);
    JS_SetPropertyStr(ctx, mouseEvent, "altKey", isAltKeyPressedJSValue);

    int isShiftKeyPressed = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    JSValue isShiftKeyPressedJSValue = JS_NewBool(ctx, isShiftKeyPressed);
    JS_SetPropertyStr(ctx, mouseEvent, "shiftKey", isShiftKeyPressedJSValue);

    int isCtrlKeyPressed = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
    JSValue isCtrlKeyPressedJSValue = JS_NewBool(ctx, isCtrlKeyPressed);
    JS_SetPropertyStr(ctx, mouseEvent, "ctrlKey", isCtrlKeyPressedJSValue);

    // Add coordinates
    Vector2 screenMousePosition = GetMousePosition();
    JSValue xScreenCoordinate = JS_NewInt32(ctx, screenMousePosition.x);
    JS_SetPropertyStr(ctx, mouseEvent, "layerX", xScreenCoordinate);
    JSValue yScreenCoordinate = JS_NewInt32(ctx, screenMousePosition.y);
    JS_SetPropertyStr(ctx, mouseEvent, "layerY", yScreenCoordinate);

    return mouseEvent;
}

void EVENT_OnMouseOver(JSContext *ctx, JSValueConst props)
{
    JSValue mouseOverFunc = JS_GetPropertyStr(ctx, props, "onMouseOver");
    if (!JS_IsFunction(ctx, mouseOverFunc))
    {
        JS_FreeValue(ctx, mouseOverFunc);
        return;
    }

    JSValue event = createMouseEvent(ctx);
    JSValue ret = JS_Call(ctx, mouseOverFunc, mouseOverFunc, 1, &event);

    JS_FreeValue(ctx, mouseOverFunc);
    JS_FreeValue(ctx, event);
    JS_FreeValue(ctx, ret);
}

void EVENT_OnHover(JSContext *ctx, JSValueConst props)
{
    EVENT_OnMouseOver(ctx, props);
}

void EVENT_HandleMouseEvents(JSContext *ctx, JSValueConst element)
{
    JSValue props = JS_GetPropertyStr(ctx, element, "props");

    if (Clay_Hovered())
        EVENT_OnHover(ctx, props);

    JS_FreeValue(ctx, props);
}