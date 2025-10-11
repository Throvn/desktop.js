#include "stdlib.h"
#include "events.h"
#include "keyboard.h"
#include "mouse.h"

int EVENT_CYCLE = 0;

/// @brief Creates a plain JS object with alt, shift & ctrl pressed properties (which is needed as a base for lots of events)
/// @param ctx
JSValue createEvent(JSContext *ctx)
{
    JSValue event = JS_NewObject(ctx);

    // Add altKey property
    int isAltKeyPressed = IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT);
    JSValue isAltKeyPressedJSValue = JS_NewBool(ctx, isAltKeyPressed);
    // Make it read only
    JS_DefinePropertyValueStr(ctx, event, "altKey", isAltKeyPressedJSValue, 0);

    int isShiftKeyPressed = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    JSValue isShiftKeyPressedJSValue = JS_NewBool(ctx, isShiftKeyPressed);
    JS_DefinePropertyValueStr(ctx, event, "shiftKey", isShiftKeyPressedJSValue, 0);

    int isCtrlKeyPressed = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
    JSValue isCtrlKeyPressedJSValue = JS_NewBool(ctx, isCtrlKeyPressed);
    JS_DefinePropertyValueStr(ctx, event, "ctrlKey", isCtrlKeyPressedJSValue, 0);

    return event;
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
void EVENT_HandleEvents(JSContext *ctx)
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
        JSValue element = findElementByKey(ctx, rootValue, id);
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
        JSValue element = elementChain[i];
        EVENT_TriggerMouseEvents(ctx, element, &stopPropagations);

        JS_FreeValue(ctx, elementChain[i]);
    }
    EVENT_HandleKeypresses(ctx);

    free(elementChain);
}

/// @brief Determines if the given element can be stored as the current focus element.
/// @brief An element CAN be focused (but is not limited to) if e.g. it listens for keyboard events or has a onFocus* callback.
/// @todo Check if this function has still a good purpose.
bool EVENT_IsElementFocusable(JSContext *ctx, JSValue element)
{
    char *neededProps[] = {
        "onKeyPress",
        "onFocus",
        "onBlur",
        "onMouseUp",
        "onMouseDown",
    };
    int neededPropsLength = sizeof(neededProps) / sizeof(neededProps[0]);

    JSValue props = JS_GetPropertyStr(ctx, element, "props");
    for (int i = 0; i < neededPropsLength; i++)
    {
        JSValue ret = JS_GetPropertyStr(ctx, props, neededProps[i]);
        if (JS_IsFunction(ctx, ret))
        {
            JS_FreeValue(ctx, props);
            JS_FreeValue(ctx, ret);
            return true;
        }
        JS_FreeValue(ctx, ret);
    }

    JS_FreeValue(ctx, props);
    return false;
}