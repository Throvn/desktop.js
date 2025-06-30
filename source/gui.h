#include <stdlib.h>
#include "../lib/quickjs/quickjs.h"
#include "../lib/clay/clay.h"
#include "../lib/raylib/src/raylib.h"

#ifndef GUI_H
#define GUI_H

typedef struct DOMStyles
{
    Clay_Color backgroundColor;
    Clay_Padding padding;
} DOMStyles;

struct DOMNode
{
    char *type;
    int key;
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

#endif