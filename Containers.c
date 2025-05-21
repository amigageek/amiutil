#include "Containers.h"

#include <ctype.h>
#include <stdarg.h>

#ifdef _POSIX_SOURCE
#   include <libgen.h>
#endif

#define VECTOR_MIN_NUM_ELEMS 8

static Status realloc_vector(Vector** new_vector_p, Vector* old_vector, size_t element_size, size_t num_elements);
static size_t round_up_pow2(size_t number);

void list_append(List* list, ListNode* node) {
    if (list->tail) {
        list->tail->next = node;
        node->prev = list->tail;
    } else {
        list->head = node;
        node->prev = nullptr;
    }

    list->tail = node;
    node->next = nullptr;  
}

void list_init(List* list) {
    list->head = nullptr;
    list->tail = nullptr;
}

void list_remove(List* list, ListNode* node) {
    if (list->head == node) {
        list->head = node->next;

        if (list->head) {
            list->head->prev = nullptr;
        }
    } else if (list->tail == node) {
        list->tail = node->prev;

        if (list->tail) {
            list->tail->next = nullptr;
        }
    } else {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    node->prev = nullptr;
    node->next = nullptr;
}

Status string_append(char** to_string_p, const char* suffix) {
    return vector_insert(to_string_p, string_length(*to_string_p), strlen(suffix), suffix);
}

Status string_clone(char** new_string_p, const char* original) {
    return string_clone_substr(new_string_p, original, strlen(original));
}

Status string_clone_substr(char** new_string_p, const char* original, size_t length) {
    TRY
    CHECK(string_new(new_string_p, length));
    memcpy(*new_string_p, original, length);

    FINALLY RETURN;
}

bool string_endswith(const char* test, const char* suffix) {
    size_t test_len = strlen(test);
    size_t suffix_len = strlen(suffix);

    return ((test_len >= suffix_len) && (strcmp(&test[test_len - suffix_len], suffix) == 0));
}

Status string_new(char** new_string_p, size_t length) {
    return vector_new(new_string_p, sizeof(char), length + 1);
}

Status string_path_append(char** base_string_p, const char* suffix) {
    TRY
    size_t base_len = string_length(*base_string_p);

    if (base_len > 0) {
        char base_last_char = (*base_string_p)[base_len - 1];

        if ((base_last_char != ':') && (base_last_char != '/')) {
            CHECK(string_append(base_string_p, "/"));
        }
    }

    CHECK(string_append(base_string_p, suffix));

    FINALLY RETURN;
}

#ifdef _POSIX_SOURCE
Status string_path_dirpart(char** new_string_p, const char* path) {
    TRY
    char* temp_path = nullptr;

    CHECK(string_clone(&temp_path, path));
    char* dir_path = dirname(temp_path);
    CHECK(string_clone(new_string_p, dir_path));

    FINALLY
    string_free(&temp_path);

    RETURN;
}
#endif

void string_free(char** string_p) {
    vector_free(string_p);
}

Status string_path_join(char** new_string_p, const char* base, const char* suffix) {
    TRY
    CHECK(string_clone(new_string_p, base));
    CHECK(string_path_append(new_string_p, suffix));

    FINALLY RETURN;
}

Status string_prepend(char** to_string_p, const char* prefix) {
    return vector_insert(to_string_p, 0, strlen(prefix), prefix);
}

Status string_printf(char** new_string_p, const char* format, ...) {
    TRY
    va_list args;
    va_start(args, format);
    int length = vsnprintf(nullptr, 0, format, args);
    va_end(args);

    ASSERT(length >= 0);
    CHECK(string_new(new_string_p, length));

    va_start(args, format);
    vsprintf(*new_string_p, format, args);
    va_end(args);

    FINALLY RETURN;
}

Status string_replace_first(char** in_string_p, const char* match, const char* with) {
    TRY
    char* match_at = strstr(*in_string_p, match);

    if (match_at) {
        size_t match_index = match_at - *in_string_p;
        size_t with_len = strlen(with);

        CHECK(vector_remove(in_string_p, match_index, strlen(match)));
        CHECK(vector_insert(in_string_p, match_index, with_len, with));
    }

    FINALLY RETURN;
}

void string_toupper(char* in_string) {
    for (char* iter = in_string; *iter; ++ iter) {
        *iter = toupper(*iter);
    }
}

Status string_truncate(char** string_p, size_t length) {
    if (length == string_length(*string_p)) {
        return (Status){StatusOK};
    }

    return vector_remove(string_p, length, string_length(*string_p) - length);
}

Status vector_append(void* to_vector_p, size_t num_elements, const void* new_elements) {
    return vector_insert(to_vector_p, vector_length(*(void**)to_vector_p), num_elements, new_elements);
}

Status vector_clear(void* vector_p) {
    return vector_remove(vector_p, 0, vector_length(*(void**)vector_p));
}

void vector_free(void* vector_p) {
    void** vector_p_typed = (void**)vector_p;

    if (*vector_p_typed) {
        free(VECTOR_FROM_STORAGE(*vector_p_typed));
        *vector_p_typed = nullptr;
    }
}

Status vector_insert(void* in_vector_p, size_t at_index, size_t num_elements, const void* new_elements) {
    TRY
    Vector* vector = VECTOR_FROM_STORAGE(*(void**)in_vector_p);
    Vector* new_vector = nullptr;
    CHECK(realloc_vector(&new_vector, vector, vector->element_size, vector->num_elements + num_elements));

    if (new_vector == vector) {
        if (at_index < vector->num_elements) {
            char* move_from = vector->storage + (at_index * vector->element_size);
            char* move_to = move_from + (num_elements * vector->element_size);
            size_t move_size = (vector->num_elements - at_index) * vector->element_size;
            memmove(move_to, move_from, move_size);
        }

        vector->num_elements += num_elements;
    } else {
        *(void**)in_vector_p = new_vector->storage;

        if (at_index > 0) {
            memcpy(new_vector->storage, vector->storage, at_index * vector->element_size);
        }

        if (at_index < vector->num_elements) {
            char* copy_from = vector->storage + (at_index * vector->element_size);
            char* copy_to = new_vector->storage + ((at_index + num_elements) * vector->element_size);
            size_t copy_size = (vector->num_elements - at_index) * vector->element_size;
            memcpy(copy_to, copy_from, copy_size);
        }

        free(vector);
    }

    char* copy_to = new_vector->storage + (at_index * new_vector->element_size);
    size_t copy_size = num_elements * new_vector->element_size;

    if (new_elements) {
        memcpy(copy_to, new_elements, copy_size);
    } else {
        memset(copy_to, 0, copy_size);
    }

    FINALLY RETURN;
}

Status vector_new(void* new_vector_p, size_t element_size, size_t num_elements) {
    TRY
    Vector* vector = nullptr;
    CHECK(realloc_vector(&vector, nullptr, element_size, num_elements));

    if (num_elements > 0) {
        memset(vector->storage, 0, num_elements * element_size);
    }

    *(void**)new_vector_p = vector->storage;

    FINALLY RETURN;
}

Status vector_remove(void* from_vector_p, size_t at_index, size_t num_elements) {
    TRY
    Vector* vector = VECTOR_FROM_STORAGE(*(void**)from_vector_p);
    Vector* new_vector = nullptr;

    CHECK(realloc_vector(&new_vector, vector, vector->element_size, vector->num_elements - num_elements));

    size_t end_index = at_index + num_elements;

    if (new_vector == vector) {
        if (end_index < vector->num_elements) {
             char* move_to = vector->storage + (at_index * vector->element_size);
             char* move_from = move_to + (num_elements * vector->element_size);
             size_t move_size = (vector->num_elements - end_index) * vector->element_size;
             memmove(move_to, move_from, move_size);
        }

        vector->num_elements -= num_elements;
    } else {
        *(void**)from_vector_p = new_vector->storage;

        if (at_index > 0) {
             memcpy(new_vector->storage, vector->storage, at_index * vector->element_size);
        }

        if (end_index < vector->num_elements) {
            char* copy_from = vector->storage + (end_index * vector->element_size);
            char* copy_to = new_vector->storage + (at_index * vector->element_size);
            size_t copy_size = (vector->num_elements - end_index) * vector->element_size;
            memcpy(copy_to, copy_from, copy_size);
        }

        free(vector);
    }

    FINALLY RETURN;
}

static Status realloc_vector(Vector** new_vector_p, Vector* old_vector, size_t element_size, size_t num_elements) {
    TRY
    size_t storage_size = round_up_pow2(MAX(VECTOR_MIN_NUM_ELEMS, num_elements) * element_size);

    if ((! old_vector) || (old_vector->storage_size != storage_size)) {
        ASSERT(*new_vector_p = malloc(sizeof(Vector) + storage_size));

        (*new_vector_p)->element_size = element_size;
        (*new_vector_p)->storage_size = storage_size;
        (*new_vector_p)->num_elements = num_elements;
    } else {
        *new_vector_p = old_vector;
    }

    FINALLY RETURN;
}

static size_t round_up_pow2(size_t number) {
    number --;
    number |= number >> 1;
    number |= number >> 2;
    number |= number >> 4;
    number |= number >> 8;
    number |= number >> 16;
#ifdef __LP64__
    number |= number >> 32;
#endif
    number ++;

    return number;
}
