#ifndef STYLES_H
#define STYLES_H

#include "../../lib/clay/clay.h"
#include "../../lib/txiki.js/src/tjs.h"

Clay_Padding STYLES_GetPadding(JSContext *ctx, JSValue element);
Clay_Color STYLES_GetColor(JSContext *ctx, JSValue element);
Clay_Color STYLES_GetBackgroundColor(JSContext *ctx, JSValue element);
int STYLES_GetFontSize(JSContext *ctx, JSValue element);
int STYLES_GetLetterSpacing(JSContext *ctx, JSValue element);
uint16_t STYLES_GetGap(JSContext *ctx, JSValue element);

#endif