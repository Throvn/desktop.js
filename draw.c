#include "gui.h"
#include "draw.h"

void GUI_RenderGroup(struct DOMNode *node)
{
    int num_keys;
    JSPropertyEnum *keys;
    int success = JS_GetOwnPropertyNames(node->ctx, &keys, &num_keys, node->properties, JS_GPN_STRING_MASK | JS_GPN_ENUM_ONLY);
    if (success != 0)
    {
        fprintf(stderr, "[Error] Could not get property names for %s %d\n", node->type, node->key);
        return;
    }

    // Apply all JSX properties to each descendant
    for (int j = 0; j < node->num_descendants; j++)
    {
        struct DOMNode *desc = node->descendants[j];
        printf("Desc: %p\n", desc);
        // Get all JSX properties
        for (int i = 0; i < num_keys; i++)
        {
            JSPropertyEnum key = keys[i];
            char *keyStr = JS_AtomToCString(node->ctx, key.atom);
            printf("Key str: %s\n", keyStr);

            // Never copy children
            if (strcmp(keyStr, "children"))
                continue;

            printf("%p %p %d | %p %p \n", desc->ctx, desc->properties, key.atom, desc->ctx, node->ctx);
            // Only add property if child prop doesn't already have it.
            JSValue keyAtom = JS_NewAtomString(desc->ctx, keyStr);
            JSAtom kA = JS_ValueToAtom(desc->ctx, keyAtom);
            if (false == JS_HasProperty(desc->ctx, desc->properties, kA))
            {
                JSValue groupPropValue = JS_GetProperty(node->ctx, node->properties, key.atom);
                JS_SetProperty(desc->ctx, desc->properties, key.atom, groupPropValue);
            }
        }
    }

    CLAY((Clay_ElementDeclaration){
        .layout = {.layoutDirection = CLAY_TOP_TO_BOTTOM}})
    {
        for (int i = 0; i < node->num_descendants; i++)
        {
            renderElement(node->descendants[i]);
        };
    }
}

int renderElement(struct DOMNode *node)
{
    if (node == NULL)
    {
        return 1;
    }

    char *type = node->type;

    JSValue idProp = JS_GetPropertyStr(node->ctx, node->properties, "id");
    char *idStr = JS_ToCString(node->ctx, idProp);
    if (strcmp(type, "group"))
    {
        GUI_RenderGroup(node);
    }

    Clay_String clString = (Clay_String){.length = strlen(idStr), .chars = idStr};
    CLAY({.layout = {.layoutDirection = CLAY_TOP_TO_BOTTOM, .padding = CLAY_PADDING_ALL(5), .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)}}})
    {
        CLAY({.layout = {.sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)}}})
        {
            CLAY_TEXT(clString, CLAY_TEXT_CONFIG({.fontSize = 24, .textColor = {255, 255, 255, 255}}));
        }

        CLAY({.layout = {.layoutDirection = CLAY_LEFT_TO_RIGHT, .padding = CLAY_PADDING_ALL(10), .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)}}})
        {
            for (int i = 0; i < node->num_descendants; i++)
            {
                renderElement(node->descendants[i]);
            };
        };
    }
    return 0;
}
