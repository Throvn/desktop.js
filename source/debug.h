#ifndef DEBUG_H
#define DEBUG_H
#include <quickjs.h>

typedef struct JSRefCountHeader
{
    int ref_count;
} JSRefCountHeader;

void JS_PrintRefCount(const char *str, JSValue v);

#endif