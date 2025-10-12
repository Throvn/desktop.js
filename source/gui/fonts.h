#ifndef FONTS_H
#define FONTS_H
#include "raylib.h"
#define FONT_MAX 24

/// @brief Includes the names of all of the already loaded fonts.
/// @brief The index of the fontName corresponts to the index in the fonts array.
extern char *fontNames[FONT_MAX];
extern Font fonts[FONT_MAX];

int FONTS_GetFontId(const char *fontFace);
void FONTS_Add(const char *fontFace, Font font);

#endif