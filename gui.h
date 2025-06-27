#ifndef GUI_H
#define GUI_H

extern struct DOMNode
{
    char *type;
    int key;
    size_t num_descendants;
    struct DOMNode **descendants;
};

#endif