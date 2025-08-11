#ifndef DRAW_H
#define DRAW_H
#include "../../lib/clay/clay.h"
#include "../../lib/txiki.js/src/tjs.h"

Clay_RenderCommandArray GUI_RenderCommands(TJSRuntime *qrt);

void GUI_RenderValue(JSContext *ctx, JSValue element);

#endif