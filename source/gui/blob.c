#include "blob.h"
#include "memory.h"
#include "draw.h"

int JS_GetBlobSize(JSContext *ctx, JSValueConst blob)
{
    JSValue sizeValue = JS_GetPropertyStr(ctx, blob, "size");
    int size;
    JS_ToInt32(ctx, &size, sizeValue);
    JS_FreeValue(ctx, sizeValue);

    return size;
}

/// Helper function to extract #parts from a Blob JSValue in QuickJS
JSValue JS_GetBlobParts(JSContext *ctx, JSValueConst blob)
{
    if (!JS_IsObject(blob))
    {
        JS_ThrowTypeError(ctx, "Invalid blob object");
        return JS_EXCEPTION;
    }
    JSValue globalThis = JS_GetGlobalObject(ctx);
    JSValue symbolInstance = JS_GetPropertyStr(ctx, globalThis, "Symbol");
    JS_FreeValue(ctx, globalThis);
    JSValue symbolForFunc = JS_GetPropertyStr(ctx, symbolInstance, "for");
    JS_FreeValue(ctx, symbolInstance);

    // Call Symbol.for('tjs.internal.blob.getParts')
    JSValue partsKey = JS_NewString(ctx, "tjs.internal.blob.getParts");
    JSValue partsSymbol = JS_Call(ctx, symbolForFunc, JS_UNDEFINED, 1, &partsKey);

    JS_FreeValue(ctx, symbolForFunc);
    JS_FreeValue(ctx, partsKey);

    JSAtom partsAtom = JS_ValueToAtom(ctx, partsSymbol);
    JSValue parts = JS_GetProperty(ctx, blob, partsAtom);

    JS_FreeValue(ctx, partsSymbol);
    JS_FreeAtom(ctx, partsAtom);

    return parts;
}

int JS_GetBlobUint8Array(JSContext *ctx, JSValueConst blob, uint8_t *out_buf)
{
    JSValue sizeValue = JS_GetPropertyStr(ctx, blob, "size");
    int size;
    JS_ToInt32(ctx, &size, sizeValue);
    JS_FreeValue(ctx, sizeValue);

    size_t offset = 0;

    JSValue partsArr = JS_GetBlobParts(ctx, blob);
    int partsArrLength = GUI_GetLength(ctx, partsArr);
    for (int i = 0; i < partsArrLength; i++)
    {
        JSValue part = JS_GetPropertyUint32(ctx, partsArr, i);
        if (JS_GetTypedArrayType(part) == JS_TYPED_ARRAY_UINT8)
        {
            size_t arrBufLength = 0;
            uint8_t *arrBuf = JS_GetUint8Array(ctx, &arrBufLength, part);
            if (!arrBuf)
            {
                JS_FreeValue(ctx, partsArr);
                JS_ThrowTypeError(ctx, "Could not render image as buffer seems corrupt");
                return 0;
            }

            memcpy(&out_buf[offset], arrBuf, arrBufLength);
            offset += arrBufLength;
        }
        else
        {
            int subBlobSize = JS_GetBlobUint8Array(ctx, part, &out_buf[offset]);
            if (subBlobSize == 0)
                return 0;
            offset += subBlobSize;
        };
        JS_FreeValue(ctx, part);
    }
    JS_FreeValue(ctx, partsArr);

    return offset;
}