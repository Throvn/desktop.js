#ifndef TREE_H
#define TREE_H
#include <quickjs.h>

typedef struct TREE_Node
{
    int key;
    JSValue value;

    struct TREE_Node *left;
    struct TREE_Node *right;
} TREE_Node;

TREE_Node *TREE_New(int key, JSValue value);

void TREE_Upsert(TREE_Node *root, int key, JSValue value);

JSValue TREE_Find(TREE_Node *root, int key);

void TREE_Print(TREE_Node *root, int depth);

#endif