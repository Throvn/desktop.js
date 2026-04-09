
#include "../../../lib/clay/clay.h"
#include "../../arena.h"
#include "../styles.h"
#include "../memory.h"
#include "../blob.h"
#include "draw.h"
#include "image.h"
#include <stdlib.h>

#define MAX_TEXTURES 2 // 64

typedef struct TextureEntry
{
    uint64_t lastUsedFrame;
    Texture2D texture;
    JSValueUnion key;
} TextureEntry;

TextureEntry textureCache[MAX_TEXTURES];
int textureCount = 0;

Texture2D *FindTexture(JSValueConst data)
{
    for (int i = 0; i < textureCount; i++)
    {
        if (textureCache[i].key.ptr == data.u.ptr)
        {
            textureCache[i].lastUsedFrame = FRAME_COUNT;
            return &textureCache[i].texture;
        }
    }
    return NULL;
}

void AddTexture(JSValueConst data, Texture2D texture)
{
    if (textureCount < MAX_TEXTURES)
    {
        textureCache[textureCount] = (TextureEntry){
            .key = data.u,
            .texture = texture,
            .lastUsedFrame = FRAME_COUNT,
        };

        textureCount++;
        return;
    }

    // Kick out oldest texture.
    int lruIndex = 0;
    uint64_t oldest = textureCache[0].lastUsedFrame;

    for (int i = 1; i < MAX_TEXTURES; i++)
    {
        if (textureCache[i].lastUsedFrame < oldest)
        {
            oldest = textureCache[i].lastUsedFrame;
            lruIndex = i;
        }
    }

    // Delete oldest texture.
    UnloadTexture(textureCache[lruIndex].texture);

    // Place new texture in oldest spot
    textureCache[lruIndex] = (TextureEntry){
        .key = data.u,
        .texture = texture,
        .lastUsedFrame = FRAME_COUNT,
    };

    return;
}

Texture2D *createTextureFromBlob(JSContext *ctx, JSValueConst data, JSValueConst element)
{
    // Is data really a Blob?
    int size = JS_GetBlobSize(ctx, data);
    uint8_t *imageData = malloc(size);
    int status = JS_GetBlobUint8Array(ctx, data, imageData);
    if (status < 0)
    {
        free(imageData);
        return NULL;
    }

    // Get the type of the Blob.
    JSValue typeValue = JS_GetPropertyStr(ctx, data, "type");
    const char *type = JS_ToCString(ctx, typeValue);
    JS_FreeValue(ctx, typeValue);
    if (strncmp(type, "image/", 6))
    {
        JS_FreeCString(ctx, type);
        free(imageData);
        GUI_RenderImagePlaceholder(ctx, element);
        return NULL;
    }

    // Now that we know the Blob type, load the blob with the correct encoding into memory.
    char *imageExt = calloc(1, strlen(type) - 4);
    imageExt[0] = '.';
    strcpy(&imageExt[1], &type[6]);
    JS_FreeCString(ctx, type);

    Image img = LoadImageFromMemory(imageExt, imageData, size);

    free(imageExt);
    free(imageData);

    Texture2D texture = LoadTextureFromImage(img);
    AddTexture(data, texture);

    UnloadImage(img);

    return FindTexture(data);
}

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

    // Check if we have texture in cache
    Texture2D *texture = FindTexture(data);
    if (texture == NULL)
    {
        // If not, cache it.
        texture = createTextureFromBlob(ctx, data, element);
        if (texture == NULL)
        {
            // If we couldn't cache it, show the placeholder.
            GUI_RenderImagePlaceholder(ctx, element);
            JS_FreeValue(ctx, data);
            return;
        }
    }
    JS_FreeValue(ctx, data);

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

    float aspectRatio = (float)texture->width / (float)texture->height;

    // If both values are not set (aka. -1)
    if (width == -1 && height == -1)
    {
        width = texture->width;
        height = texture->height;
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
}