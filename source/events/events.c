#include "events.h"

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