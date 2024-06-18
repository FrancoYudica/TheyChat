#include <stdlib.h>
#include <string.h>
#include "collections/linked_list.h"

typedef struct GenericNode {
    void* data;
    struct GenericNode* next;
} GenericNode;

struct GenericList {
    GenericNode* head;
    size_t element_size;
    size_t length;
};

struct GenericListIterator {
    GenericNode* current;
};

GenericList* generic_list_create(size_t element_size)
{
    GenericList* list = malloc(sizeof(GenericList));
    list->head = NULL;
    list->element_size = element_size;
    list->length = 0;
    return list;
}

void generic_list_destroy(GenericList* list)
{
    GenericNode* current = list->head;
    while (current != NULL) {
        GenericNode* next = current->next;
        free(current->data);
        free(current);
        current = next;
    }
    free(list);
}

void* generic_list_add(GenericList* list)
{
    GenericNode* new_node = malloc(sizeof(GenericNode));
    new_node->data = malloc(list->element_size);
    new_node->next = list->head;
    list->head = new_node;
    list->length++;
    return new_node->data;
}

bool generic_list_remove(GenericList* list, uint32_t index)
{
    if (index >= list->length) {
        return false;
    }
    GenericNode* current = list->head;
    GenericNode* prev = NULL;
    for (uint32_t i = 0; i < index; ++i) {
        prev = current;
        current = current->next;
    }
    if (prev == NULL) {
        list->head = current->next;
    } else {
        prev->next = current->next;
    }
    free(current->data);
    free(current);
    list->length--;
    return true;
}

size_t generic_list_length(GenericList* list)
{
    return list->length;
}

GenericListIterator* generic_list_iterator_create(GenericList* list)
{
    GenericListIterator* iterator = malloc(sizeof(GenericListIterator));
    iterator->current = list->head;
    return iterator;
}

void* generic_list_iterator_next(GenericListIterator* iterator)
{
    if (iterator->current != NULL) {
        void* data = iterator->current->data;
        iterator->current = iterator->current->next;
        return data;
    } else {
        return NULL;
    }
}

void generic_list_iterator_destroy(GenericListIterator* iterator)
{
    free(iterator);
}
