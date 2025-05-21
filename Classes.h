#ifndef AMIUTIL_CLASSES_H
#define AMIUTIL_CLASSES_H

#include "Application.h"

#include <intuition/classes.h>

#define DECLARE_MUI_CLASS(CLASS)                        \
    extern struct MUI_CustomClass* JOIN(_, CLASS, mcc); \
    Status JOIN(_, CLASS, init)(void);                  \
    void JOIN(_, CLASS, fini)(void);

#define DEFINE_MUI_CLASS(...) MACRO_VARIANT(_DEFINE_MUI_CLASS, __VA_ARGS__, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, _)(__VA_ARGS__)
#define _DEFINE_MUI_CLASS1(SUPER_PUBLIC) _DEFINE_MUI_CLASS(SUPER_PUBLIC, nullptr)
#define _DEFINE_MUI_CLASS2(SUPER_PUBLIC, SUPER_PRIVATE) _DEFINE_MUI_CLASS(nullptr, SUPER_PRIVATE ## _mcc)
#define _DEFINE_MUI_CLASS(SUPER_PUBLIC, SUPER_MCC)                                                  \
    DISPATCHER_PROTO();                                                                             \
    struct MUI_CustomClass* JOIN(_, CLASS, mcc);                                                    \
                                                                                                    \
    Status JOIN(_, CLASS, init)(void) {                                                             \
        TRY                                                                                         \
        ASSERT(JOIN(_, CLASS, mcc) = MUI_CreateCustomClass(                                         \
            nullptr, SUPER_PUBLIC, SUPER_MCC, sizeof(This), (void*)JOIN(_, CLASS, dispatcher)));    \
        FINALLY RETURN;                                                                             \
    }                                                                                               \
                                                                                                    \
    void JOIN(_, CLASS, fini)(void) {                                                               \
        if (JOIN(_, CLASS, mcc)) {                                                                  \
            MUI_DeleteCustomClass(JOIN(_, CLASS, mcc));                                             \
        }                                                                                           \
    }

#define METHOD(...) MACRO_VARIANT(_METHOD, __VA_ARGS__, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, _)(__VA_ARGS__)
#define _METHOD1(NAME) static Status NAME(REG(a0, struct IClass* cl), REG(a2, Object* obj))
#define _METHOD2(NAME, ARGTYPE) static Status NAME(REG(a0, struct IClass* cl), REG(a2, Object* obj), REG(a1, ARGTYPE* msg))

#define METHOD_NEW() _METHOD_NEW(CLASS)
#define _METHOD_NEW(CLASS) static Object* New(REG(a0, struct IClass* cl), REG(a2, Object* trueclass), REG(a1, struct opSet* msg))
#define METHOD_DISPOSE() _METHOD_DISPOSE(CLASS)
#define _METHOD_DISPOSE(CLASS) static uint Dispose(REG(a0, struct IClass* cl), REG(a2, Object* obj), REG(a1, Msg msg))

#define METHOD_GET() _METHOD_GET(CLASS)
#define _METHOD_GET(CLASS) static uint Get(REG(a0, struct IClass* cl), REG(a2, Object* obj), REG(a1, struct opGet* msg))
#define METHOD_SET() _METHOD_SET(CLASS)
#define _METHOD_SET(CLASS) static Status Set(REG(a0, struct IClass* cl), REG(a2, Object* obj), REG(a1, struct opSet* msg))
#define METHOD_MUI(NAME) _METHOD_MUI(NAME, CLASS)
#define _METHOD_MUI(NAME, CLASS) METHOD(NAME, JOIN(_, struct MUIP, CLASS, NAME))
#define METHOD_OVERRIDE(NAME) METHOD(NAME)
#define METHOD_OVERRIDE_MUI(NAME) METHOD(NAME, JOIN(_, struct MUIP, NAME))

#define DISPATCHER_PROTO() _DISPATCHER_PROTO(CLASS)
#define _DISPATCHER_PROTO(CLASS) uint JOIN(_, CLASS, dispatcher)(REG(a0, struct IClass* cl), REG(a2, Object* obj), REG(a1, Msg msg))

#define DISPATCHER(BODY)                        \
    DISPATCHER_PROTO() {                        \
        switch (msg->MethodID) {                \
        BODY                                    \
        }                                       \
        return DoSuperMethodA(cl, obj, msg);    \
    }

#define DISPATCH(NAME) _DISPATCH(NAME, CLASS)
#define _DISPATCH(NAME, CLASS) case JOIN(_, MUIM, CLASS, NAME): return NAME(cl, obj).v
#define DISPATCH_NEW() case OM_NEW: return (uint)New(cl, obj, (struct opSet*)msg)
#define DISPATCH_DISPOSE() case OM_DISPOSE: return Dispose(cl, obj, msg)
#define DISPATCH_GET() case OM_GET: return Get(cl, obj, (struct opGet*)msg)
#define DISPATCH_SET() case OM_SET: return Set(cl, obj, (struct opSet*)msg).v
#define DISPATCH_MUI(NAME) _DISPATCH_MUI(NAME, CLASS)
#define _DISPATCH_MUI(NAME, CLASS) case JOIN(_, MUIM, CLASS, NAME): return NAME(cl, obj, (JOIN(_, struct MUIP, CLASS, NAME)*)msg).v
#define DISPATCH_OVERRIDE(NAME) case JOIN(_, MUIM, NAME): return NAME(cl, obj).v
#define DISPATCH_OVERRIDE_MUI(NAME) case JOIN(_, MUIM, NAME): return NAME(cl, obj, (struct JOIN(_, MUIP, NAME)*)msg).v

Object* DoSuperNew(struct IClass* cl, Object* obj, uint tag1, ...);
uint get_attr(Object* object, uint attr);
Object* get_object(Object* object, uint attr);

#endif
