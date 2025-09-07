#ifndef STYLES_H
#define STYLES_H

#include "../../lib/clay/clay.h"
#include "../../lib/txiki.js/src/tjs.h"

Clay_Padding STYLES_GetPadding(JSContext *ctx, JSValueConst element);
Clay_Color STYLES_GetColor(JSContext *ctx, JSValueConst element);
Clay_Color STYLES_GetBackgroundColor(JSContext *ctx, JSValueConst element);
int STYLES_GetFontSize(JSContext *ctx, JSValueConst element);
int STYLES_GetLetterSpacing(JSContext *ctx, JSValueConst element);
uint16_t STYLES_GetGap(JSContext *ctx, JSValueConst element);
Clay_CornerRadius STYLES_GetBorderRadius(JSContext *ctx, JSValueConst element);
int STYLES_GetLineHeight(JSContext *ctx, JSValueConst element);
int STYLES_GetWidth(JSContext *ctx, JSValueConst element);
int STYLES_GetHeight(JSContext *ctx, JSValueConst element);
#endif