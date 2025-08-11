#ifndef STYLES_H
#define STYLES_H

#include "../../lib/clay/clay.h"
#include "../../lib/txiki.js/src/tjs.h"

Clay_Padding STYLES_GetPadding(JSContext *ctx, JSValue element);
Clay_Padding STYLES_GetColor(JSContext *ctx, JSValue element);
Clay_Color STYLES_GetBackgroundColor(JSContext *ctx, JSValue element);

#endif