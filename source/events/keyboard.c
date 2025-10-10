#include "events.h"
#include "keyboard.h"
#include "../gui/draw.h"

void EVENT_HandleKeypresses(JSContext *ctx)
{
    int keyCode = GetKeyPressed();

    // If no key was pressed
    if (!keyCode)
        return;

    // If no element is focused
    if (!GUI_IsElement(ctx, focusValue))
        return;

    JSValue props = JS_GetPropertyStr(ctx, focusValue, "props");
    JSValue keyPressFunc = JS_GetPropertyStr(ctx, props, "onKeyPress");
    JS_FreeValue(ctx, props);
    if (!JS_IsFunction(ctx, keyPressFunc))
    {
        JS_FreeValue(ctx, keyPressFunc);
        return;
    }

    JSValue event = createEvent(ctx);

    JSValue keyCodeValue = JS_NewInt32(ctx, keyCode);
    JS_DefinePropertyValueStr(ctx, event, "code", keyCodeValue, 0);

    JSValue ret = JS_Call(ctx, keyPressFunc, JS_UNDEFINED, 1, &event);

    JS_FreeValue(ctx, keyPressFunc);
    JS_FreeValue(ctx, ret);
}