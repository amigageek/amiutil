#ifndef AMIUTIL_APP_H
#define AMIUTIL_APP_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef FORTIFY
#include "fortify/fortify.h"
#endif

#define TRY Status status = {StatusOK};
#define FINALLY finally:
#define RETURN return status

#define THROW(STATUS)                   \
    status.v = STATUS;                  \
    goto finally

#define CHECK(EXPR)                     \
    status = EXPR;                      \
    if (status.v != StatusOK) {         \
        goto finally;                   \
    }

#define CATCH(EXPR, STATUSMASK)         \
    status = EXPR;                      \
    if (! (status.v & STATUSMASK))  {   \
        if (status.v != StatusOK) {     \
            goto finally;               \
        }                               \
    } else

#define MACRO_VARIANT(MACRO, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, VARIANT, ...) MACRO ## VARIANT
#define ASSERT(...) MACRO_VARIANT(_ASSERT, __VA_ARGS__, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, _)(__VA_ARGS__)
#define _ASSERT1(EXPR) _ASSERT2(EXPR, "Assertion failed at %s:%d", __FILE__, __LINE__)

#define _ASSERT2(EXPR, ...)             \
    if (! (EXPR)) {                     \
        status.v = StatusError;         \
        print_error(__VA_ARGS__);       \
        goto finally;                   \
    }

#define ARRAY_LENGTH(x) (sizeof(x) / sizeof(x[0]))
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define VECTOR_FROM_STORAGE(STORAGE) ((Vector*)((char*)(STORAGE) - sizeof(Vector)))

#define SWAP(x, y)                                                  \
    do {                                                            \
        char swap_temp[sizeof(x) == sizeof(y) ? sizeof(x) : -1];    \
        memcpy(swap_temp, &(y), sizeof(x));                         \
        memcpy(&(y), &(x), sizeof(x));                              \
        memcpy(&(x), swap_temp, sizeof(x));                         \
    } while(0)

typedef uint32_t uint;

typedef struct {
    enum {
        StatusError = 0,
        StatusOK = (1 << 0),
    } v;
} Status;

typedef struct ListNodeT {
    struct ListNodeT* next;
    struct ListNodeT* prev;
} ListNode;

typedef struct {
    ListNode* head;
    ListNode* tail;
} List;

typedef struct {
    size_t element_size;
    size_t num_elements;
    size_t storage_size;
    char storage[];
} Vector;

void print_error(const char* format, ...);

void list_init(List* list);
void list_append(List* list, ListNode* node);
void list_remove(List* list, ListNode* node);

Status string_append(char** to_string_p, const char* suffix);
Status string_clone(char** new_string_p, const char* original);
Status string_clone_substr(char** new_string_p, const char* original, size_t length);
bool string_endswith(const char* test, const char* suffix);
void string_free(char** string_p);
#define string_length(STRING) (vector_length(STRING) - 1)
Status string_new(char** new_string_p, size_t length);
Status string_path_append(char** base_string_p, const char* suffix);
Status string_path_dirpart(char** new_string_p, const char* path);
Status string_path_join(char** new_string_p, const char* base, const char* suffix);
Status string_prepend(char** to_string_p, const char* prefix);
Status string_printf(char** new_string_p, const char* format, ...);
Status string_replace_first(char** in_string_p, const char* match, const char* with);
void string_toupper(char* in_string);
Status string_truncate(char** string_p, size_t length);

Status vector_append(void* to_vector_p, size_t num_elements, const void* new_elements);
#define vector_foreach(VECTOR, TYPE, ITER)  \
    for (TYPE* ITER = VECTOR; ITER != VECTOR + vector_length(VECTOR); ++ ITER)
void vector_free(void* vector_p);
Status vector_insert(void* in_vector_p, size_t at_index, size_t num_elements, const void* new_elements);
#define vector_length(VECTOR) VECTOR_FROM_STORAGE(VECTOR)->num_elements
Status vector_new(void* new_vector_p, size_t element_size, size_t num_elements);
Status vector_remove(void* from_vector_p, size_t at_index, size_t num_elements);

#endif
