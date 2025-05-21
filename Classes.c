#include "Classes.h"

#include <proto/alib.h>
#include <proto/intuition.h>

Object* DoSuperNew(struct IClass* cl, Object* obj, uint tag1, ...) {
    return (Object*)DoSuperMethod(cl, obj, OM_NEW, &tag1, nullptr);
}

uint get_attr(Object* object, uint attr) {
    uint value = 0;
    GetAttr(attr, object, &value);
    return value;
}

Object* get_object(Object* object, uint attr) {
    return (Object*)get_attr(object, attr);
}
