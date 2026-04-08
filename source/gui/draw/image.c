
#include "../../../lib/clay/clay.h"
#include "../../arena.h"
#include "../styles.h"
#include "../memory.h"
#include "../blob.h"
#include "draw.h"
#include "image.h"
#include <stdlib.h>

void GUI_RenderImage(JSContext *ctx, JSValueConst element)
{
    JSValue props = JS_GetPropertyStr(ctx, element, "props");
    JSValue data = JS_GetPropertyStr(ctx, props, "data");
    if (!JS_IsObject(data))
    {
        JS_FreeValue(ctx, data);
        JS_FreeValue(ctx, props);
        GUI_RenderImagePlaceholder(ctx, element);
        return;
    }

    JS_FreeValue(ctx, props);

    int size = JS_GetBlobSize(ctx, data);
    uint8_t *imageData = malloc(size);
    int status = JS_GetBlobUint8Array(ctx, data, imageData);
    if (status < 0)
    {
        JS_FreeValue(ctx, data);
        free(imageData);
        GUI_RenderImagePlaceholder(ctx, element);
        return;
    }

    JSValue typeValue = JS_GetPropertyStr(ctx, data, "type");
    const char *type = JS_ToCString(ctx, typeValue);
    JS_FreeValue(ctx, typeValue);
    JS_FreeValue(ctx, data);
    if (strncmp(type, "image/", 6))
    {
        JS_FreeCString(ctx, type);
        free(imageData);
        GUI_RenderImagePlaceholder(ctx, element);
        return;
    }

    char *imageExt = calloc(1, strlen(type) - 4);
    imageExt[0] = '.';
    strcpy(&imageExt[1], &type[6]);
    JS_FreeCString(ctx, type);

    Image *img = malloc(sizeof(Image));
    *img = LoadImageFromMemory(imageExt, imageData, size);

    free(imageExt);
    free(imageData);

    Texture2D *texture = tex_alloc();
    *texture = LoadTextureFromImage(*img);

    // Set filter value given by props.filter
    // default to trilinear (somehow looks the best for normal photos)
    JSValue filterValue = JS_GetPropertyStr(ctx, props, "filter");
    const char *filter = JS_ToCString(ctx, filterValue);
    JS_FreeValue(ctx, filterValue);
    if (0 == strcmp(filter, "nearestNeighbor"))
    {
        SetTextureFilter(*texture, TEXTURE_FILTER_POINT);
    }
    else if (0 == strcmp(filter, "bilinear"))
    {
        SetTextureFilter(*texture, TEXTURE_FILTER_BILINEAR);
    }
    else
    {
        SetTextureFilter(*texture, TEXTURE_FILTER_TRILINEAR);
    }
    JS_FreeCString(ctx, filter);

    int width = STYLES_GetWidth(ctx, element);
    int height = STYLES_GetHeight(ctx, element);

    float aspectRatio = (float)img->width / (float)img->height;

    // If both values are not set (aka. -1)
    if (width == -1 && height == -1)
    {
        width = img->width;
        height = img->height;
    }
    else if (height < 0 && width > 0)
        height = width / aspectRatio;
    else if (width < 0 && height > 0)
        width = height * aspectRatio;

    uint32_t key = GUI_GetKey(ctx, element);

    CLAY(CLAY_IDI("Image", key), (Clay_ElementDeclaration){

                                     .image = {
                                         .imageData = texture,
                                     },
                                     .layout = {
                                         .sizing = {
                                             .height = height == -2 ? CLAY_SIZING_FIT() : (height == -1 || height == -3 ? CLAY_SIZING_GROW() : CLAY_SIZING_FIXED(height)),
                                             .width = width == -2 ? CLAY_SIZING_FIT() : (width == -1 || width == -3 ? CLAY_SIZING_GROW() : CLAY_SIZING_FIXED(width)),
                                         },
                                     },
                                 })
    {
    }

    UnloadImage(*img);
    free(img);
}