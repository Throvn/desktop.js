#ifndef RECONCILE_H
#define RECONCILE_H
#include "tjs.h"
#include "../gui/js.h"
#include "../gui/memory.h"
#include "../gui/draw.h"

void GUI_Diff(JSContext *ctx, JSValue currentTree, JSValueConst wipTree);

#endif