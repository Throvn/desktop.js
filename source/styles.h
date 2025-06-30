#ifndef STYLES_H
#define STYLES_H

#include "../lib/clay/clay.h"
#include "../lib/quickjs/quickjs.h"
#include "gui.h"

Clay_Padding parsePadding(JSContext *ctx, JSValue propValue);
Clay_Color parseColor(char *colorStr);

DOMStyles *getAllStyles(struct DOMNode *node);

#endif