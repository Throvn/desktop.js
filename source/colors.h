#ifndef COLORS_H
#define COLORS_H
#include "../lib/clay/clay.h"

enum ColorNames;
const char *COLOR_Names[];
const Clay_Color COLOR_Values[];

int COLOR_Length;
Clay_Color parseColor(char *colorStr);

#endif