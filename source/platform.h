#ifndef UTILS_H
#if defined(__APPLE__)
#include <CoreFoundation/CoreFoundation.h>
#endif

typedef struct CliArgs
{
    int count;
    const char **variables;
} CliArgs;

char *getJavaScriptSourcePath();
char *getFontSourcePath();

CliArgs *prepareArgs(int argc, const char **argv);
#endif