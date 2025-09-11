#include "mouse.h"
#include <stdlib.h>
#include "../debug.h"

#define STOP_PROPAGATION_NONE 0
#define STOP_PROPAGATION_MOUSE_UP 1 << 0
#define STOP_PROPAGATION_MOUSE_OVER 1 << 1
#define STOP_PROPAGATION_MOUSE_DOWN 1 << 2

extern JSValue rootValue;

static JSValue createMouseEvent(JSContext *ctx, const char *type)
{
    JSValue global = JS_GetGlobalObject(ctx);
    JSValue eventConstructorFunc = JS_GetPropertyStr(ctx, global, "Event");
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

    JSValue eventType = JS_NewString(ctx, type);
    JSValue event = JS_CallConstructor(ctx, eventConstructorFunc, 1, &eventType);
    int status = JS_SetPrototype(ctx, mouseEvent, event);

    JS_FreeValue(ctx, global);
    JS_FreeValue(ctx, eventConstructorFunc);
    JS_FreeValue(ctx, eventType);
    JS_FreeValue(ctx, event);

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
    if (*stopPropagations == STOP_PROPAGATION_MOUSE_UP)
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
    EVENT_OnMouseUp(ctx, element, stopPropagations);
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

    int stopPropagations = STOP_PROPAGATION_NONE;
    for (int32_t i = elementChainLength - 1; i >= 0; i--)
    {
        EVENT_TriggerMouseEvents(ctx, elementChain[i], &stopPropagations);
        JS_FreeValue(ctx, elementChain[i]);
    }

    free(elementChain);
}