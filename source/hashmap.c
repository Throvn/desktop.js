#include <stdlib.h>
#include "hashmap.h"

int fnv1a_32(char *input)
{
    int hash = FNV_OFFSET_BASIS_32;
    for (int i = 0; input[i] != '\0'; i++)
    {
        hash ^= input[i];
        hash *= FNV_PRIME_32;
    }

    return hash;
}

struct HashMap *HM_Init()
{
    int size = 32;
    struct HashMap *hm = malloc(sizeof(struct HashMap));
    hm->size = size;
    hm->count = 0;

    hm->keys = calloc(size, sizeof(char *));
    hm->values = calloc(size, sizeof(void *));

    return hm;
}

int HM_ShouldExpand(struct HashMap *hm)
{
    return (float)hm->count / hm->size > 0.75;
}

void HM_Expand(struct HashMap *hm)
{
    int oldSize = hm->size;
    char **oldKeys = hm->keys;
    void **oldValues = hm->values;

    int newSize = oldSize * 2;
    char **newKeys = calloc(oldSize * 2, sizeof(char *));
    void **newValues = calloc(oldSize * 2, sizeof(void *));
    for (int i = 0; i < hm->size; i++)
    {
        int newIndex = fnv1a_32(hm->keys[i]) % newSize;
        newKeys[newIndex] = oldKeys[i];
        newValues[newIndex] = oldValues[i];
    }
    hm->size = newSize;
    hm->keys = newKeys;
    hm->values = newValues;

    free(oldKeys);
    free(oldValues);
}

int HM_ShouldShrink(struct HashMap *hm)
{
    return hm->count > HM_MIN_SIZE && (float)hm->count / hm->size <= 0.5;
}

void HM_Shrink(struct HashMap *hm)
{
    int oldSize = hm->size;
    char **oldKeys = hm->keys;
    void **oldValues = hm->values;

    int newSize = oldSize / 2;
    char **newKeys = calloc(oldSize / 2, sizeof(char *));
    void **newValues = calloc(oldSize / 2, sizeof(void *));
    for (int i = 0; i < hm->size; i++)
    {
        int newIndex = fnv1a_32(hm->keys[i]) % newSize;
        newKeys[newIndex] = oldKeys[i];
        newValues[newIndex] = oldValues[i];
    }
    hm->size = newSize;
    hm->keys = newKeys;
    hm->values = newValues;

    free(oldKeys);
    free(oldValues);
}

void HM_Insert(struct HashMap *hm, char *key, void *value)
{
    if (HM_ShouldExpand(hm))
        HM_Expand(hm);

    int index = fnv1a_32(key) % hm->size;
    if (hm->keys[index] != 0)
    {
        exit(123);
    }

    hm->keys[index] = key;
    hm->values[index] = value;
}

void HM_Remove(struct HashMap *hm, char *key)
{
    int index = fnv1a_32(key) % hm->size;
    hm->keys[index] = NULL;
    hm->values[index] = NULL;

    if (HM_ShouldShrink(hm))
        HM_Shrink(hm);
}

void *HM_Get(struct HashMap *hm, char *key)
{
    int index = fnv1a_32(key) % hm->size;
    return hm->values[index];
}

void HM_Update(struct HashMap *hm, char *key, void *value)
{
    int index = fnv1a_32(key) % hm->size;
    hm->keys[index] = key;
    hm->values[index] = value;
}

void HM_Deinit(struct HashMap *hm)
{
    free(hm->keys);
    free(hm->values);
    free(hm);
}