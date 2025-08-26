#include "debug.h"

void JS_PrintRefCount(const char *str, JSValue v)
{
    if (JS_VALUE_GET_TAG(v) == JS_TAG_OBJECT ||
        JS_VALUE_GET_TAG(v) == JS_TAG_STRING ||
        JS_VALUE_GET_TAG(v) == JS_TAG_SYMBOL ||
        JS_VALUE_GET_TAG(v) == JS_TAG_BIG_INT)
    {

        void *ptr = JS_VALUE_GET_PTR(v);
        if (ptr)
        {
            JSRefCountHeader *hdr = (JSRefCountHeader *)ptr;
            printf("[DEBUG] %s: refcount = %d (tag=%d)\n", str, hdr->ref_count, JS_VALUE_GET_TAG(v));
        }
        else
        {
            printf("[DEBUG] %s: NULL pointer for refcount (tag=%d)\n", str, JS_VALUE_GET_TAG(v));
        }
    }
    else
    {
        printf("[DEBUG] %s: immediate value (no refcount, tag=%d)\n", str, JS_VALUE_GET_TAG(v));
    }
}