#include "platform.h"

/**
 * @brief If arguments are given, it won't do any modifications to them.
 * @brief If no args are given, it will look in the resource file and invoke the run command of the txiki.js engine by default.
 *
 */
CliArgs *prepareArgs(int argc, const char **argv)
{
    CliArgs *args = calloc(1, sizeof(CliArgs));
    args->count = argc;
    args->variables = argv;

    if (argc == 1)
    {
        args->count = 3;
        // Heap allocated
        char *path = getJavaScriptSourcePath();
        char **nargv = calloc(3, sizeof(char *));
        nargv[0] = strdup(argv[0]);
        nargv[1] = "run";
        nargv[2] = path;
        args->variables = nargv;
    }

    return args;
}

/**
 * @brief Get the Java Script Source Path object
 * @brief Only works on apple platforms currently.
 * @return char* c string of path.
 */
char *getJavaScriptSourcePath()
{
#if defined(__APPLE__)
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    if (mainBundle == NULL)
    {
        fprintf(stderr, "Could not find main bundle of app");
        exit(7);
    }

    CFStringRef name = CFSTR("index");
    CFStringRef type = CFSTR("js");
    CFURLRef url = CFBundleCopyResourceURL(mainBundle, name, type, NULL);
    if (!url)
    {
        fprintf(stderr, "Could not find index.js in bundle resources\n");
        exit(7);
    }
    CFStringRef pathStr = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
    char path[PATH_MAX];
    if (!CFStringGetCString(pathStr, path, sizeof(path), kCFStringEncodingUTF8))
    {
        fprintf(stderr, "Failed to convert CFString to C string\n");
        exit(7);
    }
    CFRelease(pathStr);
    CFRelease(url);

    return strdup(path);
#endif
}