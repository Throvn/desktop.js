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

void EVENT_OnMouseOver(Clay_ElementId elementId, Clay_PointerData pointerInfo, intptr_t userData)
{
    EventProps *eventProps = (EventProps *)userData;
    JSValue mouseOverFunc = JS_GetPropertyStr(eventProps->ctx, eventProps->props, "onMouseOver");
    if (!JS_IsFunction(eventProps->ctx, mouseOverFunc))
    {
        JS_FreeValue(eventProps->ctx, mouseOverFunc);
        free(eventProps);
        return;
    }

    JSValue event = createMouseEvent(eventProps->ctx);
    JSValue ret = JS_Call(eventProps->ctx, mouseOverFunc, mouseOverFunc, 1, &event);

    JS_FreeValue(eventProps->ctx, mouseOverFunc);
    JS_FreeValue(eventProps->ctx, event);
    JS_FreeValue(eventProps->ctx, ret);

    JS_FreeValue(eventProps->ctx, eventProps->props);
    free(eventProps);
}

void EVENT_HandleMouseEvents(JSContext *ctx, JSValueConst element)
{
    JSValue props = JS_GetPropertyStr(ctx, element, "props");
    EventProps *eventProps = calloc(1, sizeof(EventProps));
    eventProps->ctx = ctx;
    eventProps->props = props;

    Clay_OnHover(EVENT_OnMouseOver, (intptr_t)eventProps);
}