#ifndef MOUSE_H
#define MOUSE_H

#include "../../lib/clay/clay.h"
#include "../../lib/txiki.js/src/tjs.h"
#include "../gui/draw.h"
#include "../../lib/raylib/src/raylib.h"

typedef struct EventProps
{
    JSContext *ctx;
    JSValue props;
} EventProps;

void EVENT_TriggerMouseEvents(JSContext *ctx, JSValueConst element);
void EVENT_HandleMouseEvents(JSContext *ctx);

#endif