#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Forward declaration of the LinkedList structure
typedef struct LinkedList LinkedList;
typedef struct LinkedListIterator LinkedListIterator;

LinkedList* linked_list_create(size_t element_size);
void linked_list_destroy(LinkedList* list);

void* linked_list_add(LinkedList* list);
bool linked_list_remove(LinkedList* list, uint32_t index);
size_t linked_list_length(LinkedList* list);

LinkedListIterator* linked_list_iterator_create(LinkedList* list);
void* linked_list_iterator_next(LinkedListIterator* iterator);
void linked_list_iterator_destroy(LinkedListIterator* iterator);
uint32_t linked_list_iterator_index(LinkedListIterator* iterator);

#endif // linked_list_H
