#define ARENA_IMPLEMENTATION
#include "../arena.h"
#include "memory.h"

Arena arenas[2] = {{0}, {0}};
int arenaIndex = 0;

void *a_alloc(size_t size)
{
    return arena_alloc(&arenas[arenaIndex], size);
}

void a_free()
{
    return arena_free(&arenas[arenaIndex]);
}