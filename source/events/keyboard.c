#include "events.h"
#include "keyboard.h"

static JSValue createKeypressEvent(JSContext *ctx, const char *type)
{
    JSValue mouseEvent = createEvent(ctx);
    JSValue global = JS_GetGlobalObject(ctx);
    JSValue eventConstructorFunc = JS_GetPropertyStr(ctx, global, "Event");

    // Add coordinates
    Vector2 screenMousePosition = GetMousePosition();
    JSValue xScreenCoordinate = JS_NewInt32(ctx, screenMousePosition.x);
    JS_SetPropertyStr(ctx, mouseEvent, "layerX", xScreenCoordinate);
    JSValue yScreenCoordinate = JS_NewInt32(ctx, screenMousePosition.y);
    JS_SetPropertyStr(ctx, mouseEvent, "layerY", yScreenCoordinate);

    JSValue eventType = JS_NewString(ctx, type);
    JSValue event = JS_CallConstructor(ctx, eventConstructorFunc, 1, &eventType);
    JS_SetPrototype(ctx, mouseEvent, event);

    JS_FreeValue(ctx, global);
    JS_FreeValue(ctx, eventConstructorFunc);
    JS_FreeValue(ctx, eventType);
    JS_FreeValue(ctx, event);

    return mouseEvent;
}

void EVENT_HandleKeypresses(JSContext *ctx)
{
    int keyCode = GetKeyPressed();

    // If no key was pressed
    if (!keyCode)
        return;

    // If no element is focused
    if (!GUI_IsElement(ctx, focusValue))
        return;

    JSValue keyPressFunc = JS_GetPropertyStr(ctx, focusValue, "onKeyPress");
    if (!JS_IsFunction(ctx, keyPressFunc))
    {
        JS_FreeValue(ctx, keyPressFunc);
        return;
    }

    JS_Call(ctx, keyPressFunc, JS_UNDEFINED, 0, &JS_UNDEFINED);

    JS_FreeValue(ctx, keyPressFunc);
}