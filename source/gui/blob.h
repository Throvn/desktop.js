#ifndef BLOB_H
#define BLOB_H

#include "quickjs.h"

size_t JS_GetBlobSize(JSContext *ctx, JSValueConst blob);
JSValue JS_GetBlobParts(JSContext *ctx, JSValueConst blob_obj);

size_t JS_GetBlobUint8Array(JSContext *ctx, JSValueConst blob, uint8_t *out_buf);

#endif