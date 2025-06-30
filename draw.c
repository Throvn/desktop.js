#include "gui.h"
#include "draw.h"
#include "colors.h"

void renderChildren(struct DOMNode *node)
{
    for (int i = 0; i < node->num_descendants; i++)
    {
        renderElement(node->descendants[i]);
    }
}

/// @brief Handles rendering of the <group> component.
/// @note The group component only appends its props to children, so nothing is rendered.
/// @param node
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
        // Get all JSX properties
        for (int i = 0; i < num_keys; i++)
        {
            JSPropertyEnum key = keys[i];
            char *keyStr = JS_AtomToCString(node->ctx, key.atom);

            // Never copy children
            if (strcmp(keyStr, "children"))
                continue;

            // Only add property if child prop doesn't already have it.
            if (false == JS_HasProperty(desc->ctx, desc->properties, key.atom))
            {
                JSValue groupPropValue = JS_GetProperty(node->ctx, node->properties, key.atom);
                JS_SetProperty(desc->ctx, desc->properties, key.atom, groupPropValue);
            }
        }
    }

    renderChildren(node);
}

void GUI_RenderVStack(struct DOMNode *node)
{
    JSValue jsBgColor = JS_GetPropertyStr(node->ctx, node->properties, "$backgroundColor");
    Clay_Color color = {0, 0, 0, 0};
    if (JS_IsString(jsBgColor))
    {
        char *bgColorStr = JS_ToCString(node->ctx, jsBgColor);
        for (int i = 0; i < COLOR_Length; i++)
        {
            Clay_Color c = COLOR_Values[i];
            if (0 == strcmp(bgColorStr, COLOR_Names[i]))
            {
                color = COLOR_Values[i];
                break;
            }
        }
    }

    CLAY((Clay_ElementDeclaration){
        .backgroundColor = color,
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .childAlignment = {
                CLAY_ALIGN_X_CENTER,
                CLAY_ALIGN_Y_CENTER,
            },
            .sizing = {CLAY_SIZING_FIT(), CLAY_SIZING_GROW(0)},
        },
    })
    {
        renderChildren(node);
    }
}

void GUI_RenderHStack(struct DOMNode *node)
{
    CLAY((Clay_ElementDeclaration){
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .childAlignment = {
                CLAY_ALIGN_X_CENTER,
                CLAY_ALIGN_Y_CENTER,
            },
            .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_FIT()},
        },
    })
    {
        renderChildren(node);
    }
}

void GUI_RenderSpacer(struct DOMNode *node)
{
    CLAY((Clay_ElementDeclaration){
        .layout = {
            .sizing = {CLAY_SIZING_GROW(), CLAY_SIZING_GROW()},
        },
    })
    {
    }
}

void GUI_RenderString(struct DOMNode *node)
{
    JSValue jsString = JS_GetPropertyStr(node->ctx, node->properties, "value");
    char *str = JS_ToCString(node->ctx, jsString);
    Clay_String clayString = {.chars = str, .length = strlen(str)};

    CLAY((Clay_ElementDeclaration){
        .layout = {
            .sizing = {
                CLAY_SIZING_FIT(),
                CLAY_SIZING_FIT(),
            },
        },
    })
    {
        CLAY_TEXT(clayString, CLAY_TEXT_CONFIG((Clay_TextElementConfig){
                                  .fontSize = 12,
                                  .textColor = {255, 0, 0, 255},
                              }));
    }
}

int renderElement(struct DOMNode *node)
{
    if (node == NULL)
    {
        return 1;
    }

    char *type = node->type;

    if (0 == strcmp(type, "group"))
    {
        GUI_RenderGroup(node);
    }
    else if (0 == strcmp(type, "vStack"))
    {
        GUI_RenderVStack(node);
    }
    else if (0 == strcmp(type, "hStack"))
    {
        GUI_RenderHStack(node);
    }
    else if (0 == strcmp(type, "string"))
    {
        GUI_RenderString(node);
    }
    else if (0 == strcmp(type, "spacer"))
    {
        GUI_RenderSpacer(node);
    }
    else
    {
        renderChildren(node);
    }

    return 0;
}
