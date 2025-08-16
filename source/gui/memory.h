#ifndef MEMORY_H
#define MEMORY_H

extern int arenaIndex;
void *a_alloc(size_t size);
void a_free();

#endif