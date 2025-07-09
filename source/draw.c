#include "gui.h"
#include "draw.h"
#include "colors.h"
#include "styles.h"

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
    unsigned int num_keys;
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

        // Reapply all styles again. This is very expensive, we should do this on the create step!
        free(desc->styles);
        desc->styles = getAllStyles(desc);
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

    DOMStyles *styles = node->styles;
    CLAY((Clay_ElementDeclaration){
        .backgroundColor = styles->backgroundColor,
        .layout = {
            .layoutDirection = dir,
            .childAlignment = {
                CLAY_ALIGN_X_CENTER,
                CLAY_ALIGN_Y_CENTER,
            },
            .sizing = sizing,
            .padding = styles->padding,
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

    DOMStyles *styles = node->styles;
    CLAY((Clay_ElementDeclaration){
        .layout = {
            .sizing = {
                CLAY_SIZING_FIT(),
                CLAY_SIZING_FIT(),
            },
        },
    })
    {

        if (styles != NULL)
        {
            uint16_t fontSize = styles->fontSize > 0 ? styles->fontSize : 12;
            int hasColor = 0 <= (styles->color.a + styles->color.b + styles->color.g + styles->color.r);
            CLAY_TEXT(clayString, CLAY_TEXT_CONFIG((Clay_TextElementConfig){
                                      .fontSize = fontSize,
                                      .textColor = hasColor ? styles->color : (Clay_Color){0, 0, 0, 255},
                                  }));
        }
    }
}

void GUI_RenderText(struct DOMNode *node)
{

    DOMStyles *styles = node->styles;
    for (int i = 0; i < node->num_descendants; i++)
    {
        if (0 == strcmp("string", node->descendants[i]->type))
        {
            node->descendants[i]->styles = node->styles;
        }
    }
    CLAY((Clay_ElementDeclaration){
        .backgroundColor = styles->backgroundColor,
        .layout = {
            .padding = styles->padding,
        }})
    {
        for (int i = 0; i < node->num_descendants; i++)
        {
            GUI_RenderString(node->descendants[i]);
        }
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
    else if (0 == strcmp(type, "text"))
    {
        GUI_RenderText(node);
    }
    else
    {
        renderChildren(node);
    }

    return 0;
}
