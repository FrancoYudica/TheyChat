#ifndef __STRING_LIST_H__
#define __STRING_LIST_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct StringList StringList;
typedef struct StringListIterator StringListIterator;

StringList* string_list_create();
void string_list_destroy(StringList* list);

void string_list_add(StringList* list, const char* string);
bool string_list_remove(StringList* list, uint32_t index);
size_t string_list_length(StringList* list);

StringListIterator* string_list_iterator_create(StringList* list);
char* string_list_iterator_next(StringListIterator* iterator);
void string_list_iterator_destroy(StringListIterator* iterator);

#endif