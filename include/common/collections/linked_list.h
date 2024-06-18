#ifndef __GENERIC_LIST_H__
#define __GENERIC_LIST_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Forward declaration of the GenericList structure
typedef struct GenericList GenericList;
typedef struct GenericListIterator GenericListIterator;

GenericList* generic_list_create(size_t element_size);
void generic_list_destroy(GenericList* list);

void* generic_list_add(GenericList* list);
bool generic_list_remove(GenericList* list, uint32_t index);
size_t generic_list_length(GenericList* list);

GenericListIterator* generic_list_iterator_create(GenericList* list);
void* generic_list_iterator_next(GenericListIterator* iterator);
void generic_list_iterator_destroy(GenericListIterator* iterator);

#endif // GENERIC_LIST_H
