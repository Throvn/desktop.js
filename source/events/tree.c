#include "tree.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

TREE_Node *TREE_New(int key, JSValue value)
{
    TREE_Node *tree = calloc(1, sizeof(TREE_Node));
    tree->key = key;
    tree->value = value;

    return tree;
}

void TREE_Upsert(TREE_Node *root, int key, JSValue value)
{
    if (root == NULL)
        return;

    if (root->key == key)
        root->value = value;

    if (root->key > key)
    {
        if (root->left == NULL)
            root->left = TREE_New(key, value);
        else
            TREE_Upsert(root->left, key, value);
    }

    if (root->key < key)
    {
        if (root->right == NULL)
            root->right = TREE_New(key, value);
        else
            TREE_Upsert(root->right, key, value);
    }
}

JSValue TREE_Find(TREE_Node *root, int key)
{
    if (root == NULL)
        return JS_UNDEFINED;

    if (root->key == key)
        return root->value;

    if (root->key > key)
        return TREE_Find(root->left, key);

    if (root->key < key)
        return TREE_Find(root->right, key);
}

void TREE_Print(TREE_Node *root, int depth)
{
    if (root == NULL)
        return;

    // Print right subtree first for a rotated view
    TREE_Print(root->right, depth + 1);

    // Indent based on depth
    for (int i = 0; i < depth; i++)
        printf("      ");

    printf("%p\n", root->value);

    // Then print left subtree
    TREE_Print(root->left, depth + 1);
}