
#ifndef GUI_H
#define GUI_H

#include <stdlib.h>
#include "../lib/quickjs/quickjs.h"
#include "../lib/clay/clay.h"
#include "../lib/raylib/src/raylib.h"

typedef struct DOMStyles
{
    Clay_Color backgroundColor;
    Clay_Padding padding;
    Clay_Color color;
    uint16_t fontSize;
    uint16_t letterSpacing;
} DOMStyles;

struct DOMNode
{
    char *type;
    Clay_ElementId key;
    size_t num_descendants;
    struct DOMNode **descendants;

    JSContext *ctx;
    JSValue properties;

    DOMStyles *styles;
};

/// @brief Creates a Window of the desired size
/// @param width window width in pixels
/// @param height window height in pixels
/// @return All loaded fonts
Font *gui_init(int width, int height);

/// @brief Frees some Clay resources
void gui_deinit();

Clay_RenderCommandArray gui_create_render_tree();

JSModuleDef *JS_INIT_MODULE(JSContext *ctx, const char *module_name);

/// @brief Calls all js events. E.g. onMouseOver or onMouseDown for each element where the mouse is over.
void gui_fire_events();

JSValue createChildren(JSContext *ctx, int argc, JSValue argv[], struct DOMNode ***descendants_out);

#endif