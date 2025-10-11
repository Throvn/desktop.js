#ifndef EVENTS_H
#define EVENTS_H
#include "quickjs.h"
#include "raylib.h"
#include "../gui/draw.h"
#include "../gui/js.h"

#define STOP_PROPAGATION_NONE 0
#define STOP_PROPAGATION_MOUSE_UP 1 << 0
#define STOP_PROPAGATION_MOUSE_OVER 1 << 1
#define STOP_PROPAGATION_MOUSE_DOWN 1 << 2
#define STOP_PROPAGATION_FOCUS_IN 1 << 3
#define STOP_PROPAGATION_FOCUS_OUT 1 << 4

JSValue createEvent(JSContext *ctx);
void EVENT_HandleEvents(JSContext *ctx);
bool EVENT_IsElementFocusable(JSContext *ctx, JSValue element);

#endif