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
        const char **nargv = calloc(3, sizeof(char *));
        nargv[0] = strdup(argv[0]);
        nargv[1] = "run";
        nargv[2] = path;
        args->variables = nargv;
    }

    return args;
}

/**
 * @brief Get the Java Script Source Path object
 * @brief Only works on Apple platforms currently.
 * @return char* c string of path.
 */
char *getJavaScriptSourcePath()
{
#if defined(__APPLE__)
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    if (mainBundle == NULL)
    {
        fprintf(stderr, "Could not find main bundle of app");
        return NULL;
    }

    CFStringRef name = CFSTR("index");
    CFStringRef type = CFSTR("js");
    CFURLRef url = CFBundleCopyResourceURL(mainBundle, name, type, NULL);
    if (!url)
    {
        fprintf(stderr, "Could not find index.js in bundle resources\n");
        return NULL;
    }
    CFStringRef pathStr = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
    char path[PATH_MAX];
    if (!CFStringGetCString(pathStr, path, sizeof(path), kCFStringEncodingUTF8))
    {
        fprintf(stderr, "Failed to convert CFString to C string\n");
        return NULL;
    }
    CFRelease(pathStr);
    CFRelease(url);
    printf("resource url: %s\n", path);
    return strdup(path);
#endif

    return NULL;
}

/// This is only for debugging here.
/// It's not really elegant, so lets see if we can find a better solution soon.
extern CliArgs *args;

/// @brief Gets the base path of the fonts directory
/// @return NULL if not implemented for the current architecture, path with a leading slash otherwise.
/// @note Don't forget to free.
char *getFontSourcePath()
{
    int resourcePathLength = 0;
    char *jsPath = getJavaScriptSourcePath();
    if (jsPath)
    {
        // len('index.js') = 8
        resourcePathLength = strlen(jsPath) - 8;
    }
    else
    {
        // Go into development mode, and use the path supplied from the commandline.
        jsPath = calloc(1, PATH_MAX);
        strcpy(jsPath, args->variables[2]);

        // len('javascript/index.js') = 10 + 1 + 8 = 19
        resourcePathLength = strlen(jsPath) - 19;
    }

    char *path = calloc(1, PATH_MAX);
    strlcpy(path, jsPath, resourcePathLength);

    strcat(path, "/assets/fonts/");

    return path;
}