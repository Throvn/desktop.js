#include "mouse.h"
#include <stdlib.h>
#include "../debug.h"

static JSValue createMouseEvent(JSContext *ctx, const char *type)
{
    JSValue mouseEvent = createEvent(ctx);
    JSValue global = JS_GetGlobalObject(ctx);
    JSValue eventConstructorFunc = JS_GetPropertyStr(ctx, global, "Event");

    // Add coordinates
    Vector2 screenMousePosition = GetMousePosition();
    JSValue xScreenCoordinate = JS_NewInt32(ctx, screenMousePosition.x);
    JS_DefinePropertyValueStr(ctx, mouseEvent, "layerX", xScreenCoordinate, 0);
    JSValue yScreenCoordinate = JS_NewInt32(ctx, screenMousePosition.y);
    JS_DefinePropertyValueStr(ctx, mouseEvent, "layerY", yScreenCoordinate, 0);

    JSValue eventType = JS_NewString(ctx, type);
    JSValue eventPrototype = JS_GetPropertyStr(ctx, eventConstructorFunc, "prototype");
    JS_SetPrototype(ctx, mouseEvent, eventPrototype);

    JS_FreeValue(ctx, global);
    JS_FreeValue(ctx, eventConstructorFunc);
    JS_FreeValue(ctx, eventType);

    return mouseEvent;
}

JSValue js_stopPropagation(JSContext *ctx, JSValue this_val, int argc, JSValueConst argv[], int magic, JSValue *stoppedSymbol)
{
    JSAtom stoppedAtom = JS_ValueToAtom(ctx, *stoppedSymbol);
    JS_SetProperty(ctx, this_val, stoppedAtom, JS_TRUE);
    JS_FreeAtom(ctx, stoppedAtom);

    return JS_UNDEFINED;
}

static int EVENT_GetButtonPressed()
{
    int pressedLeft = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    int pressedRight = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
    int pressedMiddle = IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE);
    if (!pressedLeft && !pressedRight && !pressedMiddle)
        return -1;

    // Determines the number of the button pressed.
    // For if right was pressed, the button is 1, middle is 2, and left remains 0.
    int pressedButton = pressedLeft * 0 + pressedRight * 1 + pressedMiddle * 2;
    return pressedButton;
}

/// @brief Returns the int of the mouse button which was released (otherwise -1)
/// @return -1 => No mouse button released | 0 => left | 1 => right | 2 = middle
static int EVENT_GetButtonReleased()
{
    int pressedLeft = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
    int pressedRight = IsMouseButtonReleased(MOUSE_BUTTON_RIGHT);
    int pressedMiddle = IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE);
    if (!pressedLeft && !pressedRight && !pressedMiddle)
        return -1;

    // Determines the number of the button pressed.
    // For if right was pressed, the button is 1, middle is 2, and left remains 0.
    int pressedButton = pressedLeft * 0 + pressedRight * 1 + pressedMiddle * 2;
    return pressedButton;
}

static void EVENT_InvokeCallback(JSContext *ctx, JSValueConst element, JSValueConst callback, int stopPropagationEnum, int *stopPropagations)
{
    int pressedButton = -1;

    const char *eventType;
    switch (stopPropagationEnum)
    {
    case STOP_PROPAGATION_MOUSE_DOWN:
        eventType = "mousedown";
        pressedButton = EVENT_GetButtonPressed();
        break;
    case STOP_PROPAGATION_MOUSE_OVER:
        eventType = "mouseover";
        break;
    case STOP_PROPAGATION_MOUSE_UP:
        eventType = "mouseup";
        pressedButton = EVENT_GetButtonReleased();
        break;
    case STOP_PROPAGATION_FOCUS:
        eventType = "focus";
        break;
    case STOP_PROPAGATION_BLUR:
        eventType = "blur";
        break;
    default:
        fprintf(stderr, "[Event_HandleStopPropagation] Unknown stop propagation state");
        exit(7);
        break;
    }

    // Create a symbol where we can track if the stopPropagation function was called from js.
    JSValue stoppedSymbol = JS_DupValue(ctx, JS_NewSymbol(ctx, "stopped", false));
    JSAtom stoppedAtom = JS_ValueToAtom(ctx, stoppedSymbol);

    JSValue stopPropagationFunc = JS_NewCFunctionData(ctx, js_stopPropagation, 0, 0, 1, &stoppedSymbol);

    JSValue event = createMouseEvent(ctx, eventType);
    // Overwrite the stopPropagation function so we can check for bubbling.
    JS_SetPropertyStr(ctx, event, "stopPropagation", stopPropagationFunc);

    if (pressedButton != -1)
    {
        // Set the button property on the event object.
        JS_SetPropertyStr(ctx, event, "button", JS_NewInt32(ctx, pressedButton));
    }

    JSValue ret = JS_Call(ctx, callback, JS_UNDEFINED, 1, &event);
    JSValue stoppedValue = JS_GetProperty(ctx, event, stoppedAtom);
    bool stop = JS_ToBool(ctx, stoppedValue);
    if (stop)
        *stopPropagations |= stopPropagationEnum;

    JS_FreeValue(ctx, stoppedSymbol);
    JS_FreeAtom(ctx, stoppedAtom);
    JS_FreeValue(ctx, event);
    JS_FreeValue(ctx, ret);
    JS_FreeValue(ctx, stoppedValue);
}

static void EVENT_OnBlur(JSContext *ctx, JSValueConst element, int *stopPropagations)
{
    int elementKey = GUI_GetKey(ctx, element);
    int focusKey = GUI_GetKey(ctx, focusValue);
    if (elementKey != focusKey)
        return;

    JSValue props = JS_GetPropertyStr(ctx, element, "props");
    JSValue focusOutFunc = JS_GetPropertyStr(ctx, props, "onBlur");
    JS_FreeValue(ctx, props);

    if (!JS_IsFunction(ctx, focusOutFunc))
    {
        JS_FreeValue(ctx, focusOutFunc);
        return;
    }

    EVENT_InvokeCallback(ctx, element, focusOutFunc, STOP_PROPAGATION_BLUR, stopPropagations);
    JS_FreeValue(ctx, focusOutFunc);
}

static void EVENT_OnFocus(JSContext *ctx, JSValueConst element, int *stopPropagations)
{
    if (*stopPropagations & STOP_PROPAGATION_FOCUS)
        return;

    int releasedButton = EVENT_GetButtonReleased();
    if (releasedButton == -1)
        return;

    int elementKey = GUI_GetKey(ctx, element);
    int focusKey = GUI_GetKey(ctx, focusValue);
    *stopPropagations |= STOP_PROPAGATION_FOCUS;
    if (elementKey == focusKey)
        return;

    // make this the new focused element.
    if (EVENT_IsElementFocusable(ctx, element))
    {
        EVENT_OnBlur(ctx, focusValue, stopPropagations);
        JS_FreeValue(ctx, focusValue);
        focusValue = JS_DupValue(ctx, element);
    }

    JSValue props = JS_GetPropertyStr(ctx, element, "props");
    JSValue focusInFunc = JS_GetPropertyStr(ctx, props, "onFocus");
    JS_FreeValue(ctx, props);
    if (!JS_IsFunction(ctx, focusInFunc))
    {
        JS_FreeValue(ctx, focusInFunc);
        return;
    }

    // Call focusIn event (after focusOut of old element).
    JS_Call(ctx, focusInFunc, JS_UNDEFINED, 0, &JS_UNDEFINED);
    JS_FreeValue(ctx, focusInFunc);
}

static void EVENT_OnMouseOver(JSContext *ctx, JSValueConst element, int *stopPropagations)
{
    // If bubbling is already stopped, don't run this function.
    if (*stopPropagations & STOP_PROPAGATION_MOUSE_OVER)
        return;

    JSValue props = JS_GetPropertyStr(ctx, element, "props");

    JSValue mouseOverFunc = JS_GetPropertyStr(ctx, props, "onMouseOver");
    JS_FreeValue(ctx, props);
    if (!JS_IsFunction(ctx, mouseOverFunc))
    {
        JS_FreeValue(ctx, mouseOverFunc);
        return;
    }

    EVENT_InvokeCallback(ctx, element, mouseOverFunc, STOP_PROPAGATION_MOUSE_OVER, stopPropagations);

    JS_FreeValue(ctx, mouseOverFunc);
}

static void EVENT_OnMouseDown(JSContext *ctx, JSValueConst element, int *stopPropagations)
{
    if (*stopPropagations & STOP_PROPAGATION_MOUSE_DOWN)
        return;

    int pressedButton = EVENT_GetButtonPressed();
    if (pressedButton == -1)
        return;

    JSValue props = JS_GetPropertyStr(ctx, element, "props");

    JSValue mouseDownFunc = JS_GetPropertyStr(ctx, props, "onMouseDown");
    JS_FreeValue(ctx, props);
    if (!JS_IsFunction(ctx, mouseDownFunc))
    {
        JS_FreeValue(ctx, mouseDownFunc);
        return;
    }

    EVENT_InvokeCallback(ctx, element, mouseDownFunc, STOP_PROPAGATION_MOUSE_DOWN, stopPropagations);

    JS_FreeValue(ctx, mouseDownFunc);
}

static void EVENT_OnMouseUp(JSContext *ctx, JSValueConst element, int *stopPropagations)
{
    if (*stopPropagations & STOP_PROPAGATION_MOUSE_UP)
        return;

    int releasedButton = EVENT_GetButtonReleased();
    if (releasedButton == -1)
        return;

    JSValue props = JS_GetPropertyStr(ctx, element, "props");
    JSValue mouseUpFunc = JS_GetPropertyStr(ctx, props, "onMouseUp");
    JS_FreeValue(ctx, props);
    if (!JS_IsFunction(ctx, mouseUpFunc))
    {
        JS_FreeValue(ctx, mouseUpFunc);
        return;
    }

    EVENT_InvokeCallback(ctx, element, mouseUpFunc, STOP_PROPAGATION_MOUSE_UP, stopPropagations);

    JS_FreeValue(ctx, mouseUpFunc);
}

void EVENT_TriggerMouseEvents(JSContext *ctx, JSValueConst element, int *stopPropagations)
{
    EVENT_OnMouseOver(ctx, element, stopPropagations);
    EVENT_OnMouseDown(ctx, element, stopPropagations);
    EVENT_OnFocus(ctx, element, stopPropagations);
    EVENT_OnMouseUp(ctx, element, stopPropagations);
}
