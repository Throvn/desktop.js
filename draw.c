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
            JS_FreeCString(node->ctx, keyStr);

            // Never copy children
            if (0 == strcmp(keyStr, "children"))
                continue;

            // Only add property if child prop doesn't already have it.
            if (false == JS_HasProperty(desc->ctx, desc->properties, key.atom))
            {
                JSValue groupPropValue = JS_GetProperty(node->ctx, node->properties, key.atom);
                success = JS_SetProperty(desc->ctx, desc->properties, key.atom, groupPropValue);
                printf("Property success: %d\n", success);
                if (success != 1)
                {
                    printf("[Warning] could not set property in group to %s\n", keyStr);
                    exit(2);
                }
            }
        }
    }
    JS_FreePropertyEnum(node->ctx, keys, num_keys);

    renderChildren(node);
}

void GUI_RenderStack(struct DOMNode *node, char direction)
{
    // Determine layout direction.
    Clay_Sizing sizing;
    int dir;
    switch (direction)
    {
    case 'v':
        sizing = (Clay_Sizing){CLAY_SIZING_FIT(), CLAY_SIZING_GROW()};
        dir = CLAY_TOP_TO_BOTTOM;
        break;
    case 'h':
        sizing = (Clay_Sizing){CLAY_SIZING_GROW(), CLAY_SIZING_FIT()};
        dir = CLAY_LEFT_TO_RIGHT;
        break;
    default:
        fprintf(stderr, "[GUI_RenderStack] Stack direction '%c' unknown", direction);
        exit(2);
    }

    JSValue jsBgColor = JS_GetPropertyStr(node->ctx, node->properties, "$backgroundColor");
    char *colorStr = JS_ToCString(node->ctx, jsBgColor);
    // printf("$backgroundColor for %cStack is %s", direction, colorStr);
    Clay_Color color = parseColor(colorStr);
    CLAY((Clay_ElementDeclaration){
        .backgroundColor = color,
        .layout = {
            .layoutDirection = dir,
            .childAlignment = {
                CLAY_ALIGN_X_CENTER,
                CLAY_ALIGN_Y_CENTER,
            },
            .sizing = sizing,
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
        GUI_RenderStack(node, 'v');
    }
    else if (0 == strcmp(type, "hStack"))
    {
        GUI_RenderStack(node, 'h');
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
