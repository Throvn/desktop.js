#include "mouse.h"

extern JSValue rootValue;

JSValue findValueById(JSContext *ctx, JSValue element, int searchKey)
{
    int key = GUI_GetKey(ctx, element);
    if (key == searchKey)
        return element;

    JSValue children = GUI_GetChildren(ctx, element);
    int num_children = GUI_GetLength(ctx, children);
    for (int i = 0; i < num_children; i++)
    {
        JSValue child = JS_GetPropertyUint32(ctx, children, i);
        JSValue foundValue = findValueById(ctx, child, searchKey);
        if (!JS_IsUndefined(foundValue))
            return foundValue;
    }

    return JS_UNDEFINED;
}

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

void EVENTS_InvokeMouseOver(JSContext *ctx, JSValue element)
{
    Vector2 delta = GetMouseDelta();
    if (0 == delta.x + delta.y)
        return;

    JSValue props = JS_GetPropertyStr(ctx, element, "props");
    if (JS_IsUndefined(props))
        return;

    JSValue mouseOverValue = JS_GetPropertyStr(ctx, props, "onMouseOver");
    if (!JS_IsFunction(ctx, mouseOverValue))
        return;

    JSValue mouseEvent = createMouseEvent(ctx);
    JS_Call(ctx, mouseOverValue, mouseOverValue, 1, &mouseEvent);
}

void EVENTS_Invoke(TJSRuntime *qrt)
{
    if (JS_IsUninitialized(rootValue))
        return;

    Clay_ElementIdArray ids = Clay_GetPointerOverIds();
    if (ids.length < 1)
        return;

    JSContext *ctx = TJS_GetJSContext(qrt);

    // Start from smallest node first (most nested element, gets mouse events first)
    for (int i = ids.length - 1; 0 <= i; i--)
    {
        JSValue element = findValueById(ctx, rootValue, ids.internalArray[i].id);

        if (JS_IsUndefined(element))
            continue;

        EVENTS_InvokeMouseOver(ctx, element);
    }
}