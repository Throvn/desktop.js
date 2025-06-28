#ifndef GUI_H
#define GUI_H

extern struct DOMNode
{
    char *type;
    int key;
    size_t num_descendants;
    struct DOMNode **descendants;
};

/// @brief Creates a Window of the desired size
/// @param width window width in pixels
/// @param height window height in pixels
void gui_init(int width, int height);

/// @brief Frees some Clay resources
void gui_deinit();

#endif