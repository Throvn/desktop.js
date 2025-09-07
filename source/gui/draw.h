#ifndef DRAW_H
#define DRAW_H
#include "../../lib/clay/clay.h"
#include "../../lib/txiki.js/src/tjs.h"
#include "memory.h"

Clay_RenderCommandArray GUI_RenderCommands(TJSRuntime *qrt);

void GUI_RenderValue(JSContext *ctx, JSValue element);

void GUI_PrintKeys(JSContext *ctx, JSValueConst element);
bool GUI_IsElement(JSContext *ctx, JSValueConst element);
int GUI_GetLength(JSContext *ctx, JSValueConst element);
uint32_t GUI_GetKey(JSContext *ctx, JSValueConst element);
JSValue GUI_GetChildren(JSContext *ctx, JSValueConst element);

#endif