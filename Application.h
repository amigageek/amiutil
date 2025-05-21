#ifndef AMIUTIL_APPLICATION_H
#define AMIUTIL_APPLICATION_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef FORTIFY
#   include "Fortify/Fortify.h"
#endif

#define nullptr NULL

#define TRY Status status = {StatusOK};
#define FINALLY finally:
#define RETURN return status

#define THROW(STATUS)   \
    status.v = STATUS;  \
    goto finally

#define CHECK(EXPR)             \
    status = EXPR;              \
    if (status.v != StatusOK) { \
        goto finally;           \
    }

#define CHECK_METHOD(OBJ, ...) CHECK((Status){DoMethod(OBJ, __VA_ARGS__)})

#define RETURN_NOTIFY                                                                   \
    if (status.v != StatusOK) {                                                         \
        DoMethod(_app(obj), MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit); \
    }                                                                                   \
    return status

#define MACRO_VARIANT(MACRO, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, VARIANT, ...) MACRO ## VARIANT

#define ASSERT(...) MACRO_VARIANT(_ASSERT, __VA_ARGS__, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, _)(__VA_ARGS__)
#define _ASSERT1(EXPR) _ASSERT2(EXPR, "Assertion failed at %s:%d", __FILE__, __LINE__)
#define _ASSERT2(EXPR, ...)         \
    if (! (EXPR)) {                 \
        status.v = StatusError;     \
        print_error(__VA_ARGS__);   \
        goto finally;               \
    }

#define ASSERT_OR(EXPR, STATUS) \
    if (! (EXPR)) {             \
        status.v = STATUS;      \
        goto finally;           \
    }

#define CATCH(...) MACRO_VARIANT(_CATCH, __VA_ARGS__, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, _)(__VA_ARGS__)
#define _CATCH1(STATUSMASK) if ((status.v & STATUSMASK) && (status.v = StatusOK))
#define _CATCH2(EXPR, STATUSMASK)       \
    status = EXPR;                      \
    if (! (status.v & STATUSMASK))  {   \
        if (status.v != StatusOK) {     \
            goto finally;               \
        }                               \
    } else

#define ARRAY_LENGTH(x) (sizeof(x) / sizeof(x[0]))
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

#define SWAP(x, y)                                                  \
    do {                                                            \
        char swap_temp[sizeof(x) == sizeof(y) ? sizeof(x) : -1];    \
        memcpy(swap_temp, &(y), sizeof(x));                         \
        memcpy(&(y), &(x), sizeof(x));                              \
        memcpy(&(x), swap_temp, sizeof(x));                         \
    } while(0)

#define JOIN(...) MACRO_VARIANT(_JOIN, __VA_ARGS__, 3, 3, 3, 3, 3, 3, 3, 2, _, _, _)(__VA_ARGS__)
#define _JOIN2(SEP, X, Y) X ## SEP ## Y
#define _JOIN3(SEP, X, Y, Z) X ## SEP ## Y ## SEP ## Z

#ifdef __GNUC__
#define REG(reg, arg) arg __asm(#reg)
#else
#define REG(reg, arg) __reg(#reg) arg
#endif

typedef uint8_t uchar;
typedef uint16_t ushort;
typedef uint32_t uint;

typedef struct {
    enum {
        StatusError = 0,
        StatusOK = (1 << 0),
        StatusIO = (1 << 1),
    } v;
} Status;

void check_stack(void);
void print_error(const char* format, ...);

#include "Classes.h"
#include "Containers.h"

#endif
