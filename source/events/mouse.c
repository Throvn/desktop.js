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

static void EVENT_OnMouseOver(JSContext *ctx, JSValueConst props)
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

static void EVENT_OnMouseDown(JSContext *ctx, JSValueConst props)
{
    int pressedLeft = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    int pressedRight = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
    int pressedMiddle = IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE);
    if (!pressedLeft && !pressedRight && !pressedMiddle)
        return;

    // Determines the number of the button pressed.
    // For if right was pressed, the button is 1, middle is 2, and left remains 0.
    int pressedButton = pressedLeft * 0 + pressedRight * 1 + pressedMiddle * 2;

    JSValue clickFunc = JS_GetPropertyStr(ctx, props, "onMouseDown");
    if (!JS_IsFunction(ctx, clickFunc))
    {
        JS_FreeValue(ctx, clickFunc);
        return;
    }

    JSValue event = createMouseEvent(ctx);
    // Set the button property on the event object.
    JS_SetPropertyStr(ctx, event, "button", JS_NewInt32(ctx, pressedButton));

    JSValue ret = JS_Call(ctx, clickFunc, clickFunc, 1, &event);

    JS_FreeValue(ctx, clickFunc);
    JS_FreeValue(ctx, event);
    JS_FreeValue(ctx, ret);
}

static void EVENT_OnMouseUp(JSContext *ctx, JSValueConst props)
{
    int pressedLeft = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
    int pressedRight = IsMouseButtonReleased(MOUSE_BUTTON_RIGHT);
    int pressedMiddle = IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE);
    if (!pressedLeft && !pressedRight && !pressedMiddle)
        return;

    // Determines the number of the button pressed.
    // For if right was pressed, the button is 1, middle is 2, and left remains 0.
    int pressedButton = pressedLeft * 0 + pressedRight * 1 + pressedMiddle * 2;

    JSValue clickFunc = JS_GetPropertyStr(ctx, props, "onMouseUp");
    if (!JS_IsFunction(ctx, clickFunc))
    {
        JS_FreeValue(ctx, clickFunc);
        return;
    }

    JSValue event = createMouseEvent(ctx);
    // Set the button property on the event object.
    JS_SetPropertyStr(ctx, event, "button", JS_NewInt32(ctx, pressedButton));

    JSValue ret = JS_Call(ctx, clickFunc, clickFunc, 1, &event);

    JS_FreeValue(ctx, clickFunc);
    JS_FreeValue(ctx, event);
    JS_FreeValue(ctx, ret);
}

void EVENT_TriggerMouseEvents(JSContext *ctx, JSValueConst element)
{
    JSValue props = JS_GetPropertyStr(ctx, element, "props");

    EVENT_OnMouseOver(ctx, props);
    EVENT_OnMouseDown(ctx, props);
    EVENT_OnMouseUp(ctx, props);

    JS_FreeValue(ctx, props);
}

JSValue findElementByKey(JSContext *ctx, JSValueConst element, uint32_t id)
{
    uint32_t key = GUI_GetKey(ctx, element);
    if (key == id)
        return JS_DupValue(ctx, element);

    JSValue renderChild = JS_GetPropertyStr(ctx, element, "_renderChild");
    if (GUI_IsElement(ctx, renderChild))
    {
        JSValue found = findElementByKey(ctx, renderChild, id);
        JS_FreeValue(ctx, renderChild);
        return found;
    }
    JS_FreeValue(ctx, renderChild);

    JSValue children = GUI_GetChildren(ctx, element);
    int childrenLength = GUI_GetLength(ctx, children);
    for (int i = 0; i < childrenLength; i++)
    {
        JSValue child = JS_GetPropertyUint32(ctx, children, i);
        JSValue found = findElementByKey(ctx, child, id);
        if (!JS_IsUndefined(found))
        {
            JS_FreeValue(ctx, children);
            JS_FreeValue(ctx, child);
            return found;
        }
        JS_FreeValue(ctx, child);
    }
    JS_FreeValue(ctx, children);

    return JS_UNDEFINED;
}

/// Call once per c event loop execution.
/// Gets all Clay_ElementId s and calls the correct JS callbacks for the elements.
void EVENT_HandleMouseEvents(JSContext *ctx)
{
    if (!GUI_IsElement(ctx, rootValue))
        return;

    Clay_ElementIdArray ids = Clay_GetPointerOverIds();

    JSValue *elementChain = calloc(ids.length + 1, sizeof(JSValue));
    elementChain[0] = JS_DupValue(ctx, rootValue);
    int elementChainLength = 1;
    for (int32_t i = 0; i < ids.length; i++)
    {
        uint32_t id = ids.internalArray[i].offset;
        JSValue element = findElementByKey(ctx, elementChain[elementChainLength - 1], id);
        if (GUI_IsElement(ctx, element))
        {
            elementChain[elementChainLength] = element;
            elementChainLength += 1;
            continue;
        }
        JS_FreeValue(ctx, element);
    }

    for (int32_t i = elementChainLength - 1; i >= 0; i--)
    {
        EVENT_TriggerMouseEvents(ctx, elementChain[i]);
        JS_FreeValue(ctx, elementChain[i]);
    }

    free(elementChain);
}