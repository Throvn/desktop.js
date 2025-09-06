#define ARENA_IMPLEMENTATION
#include "../arena.h"
#include "memory.h"

Arena arenas[2] = {{0}, {0}};
int arenaIndex = 0;

void *a_alloc(size_t size)
{
    return arena_alloc(&arenas[arenaIndex], size);
}

Arena textureArena = {0};
void a_free()
{
    tex_free();
    arena_free(&arenas[arenaIndex]);
}

Texture2D *tex_alloc()
{
    void *ptr = arena_alloc(&textureArena, sizeof(Texture2D));
    return ptr;
}

void tex_free()
{
    size_t texSize = (sizeof(Texture2D) + sizeof(uintptr_t) - 1) / sizeof(uintptr_t);

    for (Region *r = textureArena.begin; r != NULL; r = r->next)
    {
        size_t count = r->count / texSize; // number of textures in this region
        Texture2D *base = (Texture2D *)r->data;

        for (size_t i = 0; i < count; i++)
        {
            UnloadTexture(base[i]); // unload each texture
        }
    }
    arena_reset(&textureArena);
}