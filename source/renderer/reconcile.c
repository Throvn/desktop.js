#include "reconcile.h"

void GUI_DiffChildren(JSContext *ctx, JSValue currentElement, JSValue wipElement)
{
    JSValue cChildren = GUI_GetChildren(ctx, currentElement);
    JSValue wipChildren = GUI_GetChildren(ctx, wipElement);

    int wipChildrenLength = GUI_GetLength(ctx, wipChildren);
    for (int i = 0; i < wipChildrenLength; i++)
    {

        JSValue cChild = JS_GetPropertyUint32(ctx, cChildren, i);
        JSValue wipChild = JS_GetPropertyUint32(ctx, wipChildren, i);
        GUI_Diff(ctx, cChild, wipChild);
    }
}

void GUI_Diff(JSContext *ctx, JSValue currentElement, JSValue wipElement)
{
    if (JS_IsUndefined(currentElement) || !JS_IsObject(wipElement))
        return;

    JSValue cType = JS_GetPropertyStr(ctx, currentElement, "type");
    JSValue wipType = JS_GetPropertyStr(ctx, wipElement, "type");

    bool isTypeEqual = JS_IsStrictEqual(ctx, cType, wipType);
    JS_FreeValue(ctx, wipType);

    const char *cTypeString = JS_ToCString(ctx, cType);
    JS_FreeValue(ctx, cType);
    if (isTypeEqual && 0 == strcmp(cTypeString, "custom"))
    {
        JSValue cInstance = JS_GetPropertyStr(ctx, currentElement, "instance");
        JS_SetPropertyStr(ctx, wipElement, "instance", cInstance);
    }

    if (isTypeEqual)
    {
        GUI_DiffChildren(ctx, currentElement, wipElement);
    }
}
