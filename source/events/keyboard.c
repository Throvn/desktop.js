#include "events.h"
#include "keyboard.h"
#include "../gui/draw.h"

/// @brief Turns the raylib key code into a human readable string
/// @param keyCode
/// @return static string, or string allocated in the arena. (Don't worry about clearing)
static char *getKeyName(int keyCode)
{
    bool isAsciiPrintable = keyCode >= 65 && keyCode <= 125;
    if (isAsciiPrintable)
    {
        char keyC = (char)keyCode;
        char str[] = {keyC, '\0'};
        char *heapStr = a_alloc(2);
        strcpy(heapStr, str);
        return heapStr;
    }

    switch (keyCode)
    {
    // Start Modifier keys
    case KEY_LEFT_ALT:
    case KEY_RIGHT_ALT:
        return "Alt";
    case KEY_CAPS_LOCK:
        return "CapsLock";
    case KEY_LEFT_CONTROL:
    case KEY_RIGHT_CONTROL:
        return "Control";
    case KEY_LEFT_SUPER:
    case KEY_RIGHT_SUPER:
        return "Super";
    case KEY_LEFT_SHIFT:
    case KEY_RIGHT_SHIFT:
        return "Shift";
    // End Modifier keys
    // Start Whitespace keys
    case KEY_ENTER:
        return "Enter";
    case KEY_TAB:
        return "Tab";
    case KEY_SPACE:
        return " ";
    // End Whitespace keys
    // Start Navigation keys
    case KEY_DOWN:
        return "ArrowDown";
    case KEY_LEFT:
        return "ArrowLeft";
    case KEY_RIGHT:
        return "ArrowRight";
    case KEY_UP:
        return "ArrowUp";
    case KEY_END:
        return "End";
    case KEY_HOME:
        return "Home";
    case KEY_PAGE_DOWN:
        return "PageDown";
    case KEY_PAGE_UP:
        return "PageUp";
    // End Navigation keys
    // Start Editing keys
    case KEY_BACKSPACE:
        return "Backspace";
    case KEY_DELETE:
        return "Delete";
    case KEY_INSERT:
        return "Insert";
    // End Editing keys
    // Start Function keys
    case KEY_F1:
        return "F1";
    case KEY_F2:
        return "F2";
    case KEY_F3:
        return "F3";
    case KEY_F4:
        return "F5";
    case KEY_F6:
        return "F6";
    case KEY_F7:
        return "F7";
    case KEY_F8:
        return "F8";
    case KEY_F9:
        return "F9";
    case KEY_F10:
        return "F10";
    case KEY_F11:
        return "F11";
    case KEY_F12:
        return "F12";
    // End Function keys
    // Start Audio Control keys
    case KEY_VOLUME_UP:
        return "AudioVolumeUp";
    case KEY_VOLUME_DOWN:
        return "AudioVolumeDown";
    // End Audio Control keys
    default:
        return "Unidentified";
    }
}

void EVENT_HandleKeypresses(JSContext *ctx)
{
    int keyCode = GetCharPressed();
    if (!keyCode)
        keyCode = GetKeyPressed();

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

    char *keyName = getKeyName(keyCode);
    JSValue keyValue = JS_NewString(ctx, keyName);
    JS_DefinePropertyValueStr(ctx, event, "key", keyValue, 0);

    JSValue ret = JS_Call(ctx, keyPressFunc, JS_UNDEFINED, 1, &event);

    JS_FreeValue(ctx, event);
    JS_FreeValue(ctx, keyPressFunc);
    JS_FreeValue(ctx, ret);
}