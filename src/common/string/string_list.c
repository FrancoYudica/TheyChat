#include "string/string_list.h"
#include "collections/linked_list.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct StringList {
    LinkedList* list;
};

struct StringListIterator {
    LinkedListIterator* it;
};

StringList* string_list_create()
{
    StringList* str_list = (StringList*)malloc(sizeof(StringList*));
    if (str_list == NULL) {
        perror("Failed to initialize list");
        exit(EXIT_FAILURE);
    }
    str_list->list = linked_list_create(sizeof(char*));
    if (str_list->list == NULL) {
        perror("Failed to initialize list");
        exit(EXIT_FAILURE);
    }

    return str_list;
}
void string_list_destroy(StringList* list)
{
    StringListIterator* it = string_list_iterator_create(list);
    char* str = NULL;
    while ((str = string_list_iterator_next(it)) != NULL) {
        free(str);
    }

    linked_list_destroy(list->list);
}

void string_list_add(StringList* list, const char* string)
{
    char** str_ptr = (char**)linked_list_add(list->list);

    // Allocates memory for string
    *str_ptr = (char*)malloc(strlen(string) + 1);

    // Copies
    strcpy(*str_ptr, string);
}

bool string_list_remove(StringList* list, uint32_t index)
{
    return linked_list_remove(list->list, index);
}

size_t string_list_length(StringList* list)
{
    return linked_list_length(list->list);
}

StringListIterator* string_list_iterator_create(StringList* list)
{
    StringListIterator* str_it = (StringListIterator*)malloc(sizeof(StringListIterator));
    str_it->it = linked_list_iterator_create(list->list);
    return str_it;
}
char* string_list_iterator_next(StringListIterator* iterator)
{
    void* next = linked_list_iterator_next(iterator->it);
    if (next == NULL)
        return NULL;

    return *(char**)next;
}
void string_list_iterator_destroy(StringListIterator* iterator)
{
    linked_list_iterator_destroy(iterator->it);
    free(iterator);
}