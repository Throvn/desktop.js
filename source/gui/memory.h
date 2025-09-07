#ifndef MEMORY_H
#define MEMORY_H
#include "raylib.h"

extern int arenaIndex;
void *a_alloc(size_t size);
void a_free();

Texture2D *tex_alloc();
void tex_free();

#endif