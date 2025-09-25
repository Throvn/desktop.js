#ifndef GUI_JS_H
#define GUI_JS_H
#include <quickjs.h>

JSModuleDef *GUI_js_init_module(JSContext *ctx);

extern JSValue rootValue;
extern JSValue focusValue;

#endif