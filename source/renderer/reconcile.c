#include "reconcile.h"
#include "../debug.h"
#include <stdlib.h>

void GUI_DiffChildren(JSContext *ctx, JSValueConst currentElement, JSValueConst wipElement)
{
    JSValue cChildren = GUI_GetChildren(ctx, currentElement);
    JSValue wipChildren = GUI_GetChildren(ctx, wipElement);

    int wipChildrenLength = GUI_GetLength(ctx, wipChildren);
    int cChildrenLength = GUI_GetLength(ctx, cChildren);
    int maxLength = (wipChildrenLength > cChildrenLength ? wipChildrenLength : cChildrenLength);
    for (int i = 0; i < maxLength; i++)
    {

        JSValue cChild = JS_GetPropertyUint32(ctx, cChildren, i);
        JSValue wipChild = JS_GetPropertyUint32(ctx, wipChildren, i);
        GUI_Diff(ctx, cChild, wipChild);

        JS_FreeValue(ctx, cChild);
        JS_FreeValue(ctx, wipChild);
    }

    JS_FreeValue(ctx, cChildren);
    JS_FreeValue(ctx, wipChildren);
}

void GUI_Diff(JSContext *ctx, JSValueConst currentElement, JSValueConst wipElement)
{
    if (JS_IsUndefined(currentElement) || !JS_IsObject(wipElement))
    {
        return;
    }

    JSValue cType = JS_GetPropertyStr(ctx, currentElement, "type");
    JSValue wipType = JS_GetPropertyStr(ctx, wipElement, "type");

    bool isTypeEqual = JS_IsStrictEqual(ctx, cType, wipType);

    const char *cTypeString = JS_ToCString(ctx, cType);
    if (isTypeEqual && 0 == strcmp(cTypeString, "custom"))
    {
        JSValue cInstance = JS_GetPropertyStr(ctx, currentElement, "instance");
        JS_SetPropertyStr(ctx, wipElement, "instance", cInstance);
    }
    else if (isTypeEqual)
    {
        GUI_DiffChildren(ctx, currentElement, wipElement);
    }

    JS_FreeCString(ctx, cTypeString);
    JS_FreeValue(ctx, wipType);
    JS_FreeValue(ctx, cType);
}
