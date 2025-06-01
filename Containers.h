#ifndef AMIUTIL_CONTAINERS_H
#define AMIUTIL_CONTAINERS_H

#include "Application.h"

#define VECTOR_FROM_STORAGE(STORAGE) ((Vector*)((char*)(STORAGE) - sizeof(Vector)))

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

void list_init(List* list);
void list_append(List* list, ListNode* node);
void list_remove(List* list, ListNode* node);

Status string_append(char** to_string_p, const char* suffix);
Status string_clone(char** new_string_p, const char* original);
Status string_clone_substr(char** new_string_p, const char* original, size_t length);
uint string_count_substr(const char* test, const char* substr);
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
Status string_replace_all(char** in_string_p, const char* match, const char* with);
bool string_startswith(const char* test, const char* prefix);
void string_tolower(char* in_string);
void string_toupper(char* in_string);
Status string_truncate(char** string_p, size_t length);

Status vector_append(void* to_vector_p, size_t num_elements, const void* new_elements);
Status vector_clear(void* vector_p);
#define vector_foreach(VECTOR, TYPE, ITER)  \
    for (TYPE* ITER = VECTOR; ITER != VECTOR + vector_length(VECTOR); ++ ITER)
void vector_free(void* vector_p);
Status vector_insert(void* in_vector_p, size_t at_index, size_t num_elements, const void* new_elements);
#define vector_last(VECTOR) VECTOR[VECTOR_FROM_STORAGE(VECTOR)->num_elements - 1]
#define vector_length(VECTOR) VECTOR_FROM_STORAGE(VECTOR)->num_elements
Status vector_new(void* new_vector_p, size_t element_size, size_t num_elements);
Status vector_remove(void* from_vector_p, size_t at_index, size_t num_elements);

#endif
