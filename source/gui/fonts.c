#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include "fonts.h"

char *fontNames[FONT_MAX] = {0};
Font fonts[FONT_MAX] = {0};
int fontLength = 0;

void FONTS_Add(const char *fontFace, Font font)
{
    if (fontFace == NULL)
        return;

    if (fontLength >= FONT_MAX)
    {
        printf("Maximum number of fonts loaded. Try using less font faces (<24).\n");
        exit(7);
    }

    fontNames[fontLength] = strdup(fontFace);
    fonts[fontLength] = font;
    fontLength += 1;
}

int FONTS_GetFontId(const char *fontFace)
{
    for (size_t i = 0; i < fontLength; i++)
    {
        if (!strcmp(fontNames[i], fontFace))
            return i;
    }
    return -1;
}