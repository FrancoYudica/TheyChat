#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// Node structure for the linked list
typedef struct Node {
    void* data;
    struct Node* next;
} Node;

// GenericList structure
typedef struct GenericList {
    Node* head;
    Node* tail;
    size_t element_size;
    size_t size;
} GenericList;

// GenericListIterator structure
typedef struct GenericListIterator {
    Node* current;
    size_t element_size;
} GenericListIterator;

// Creates a new generic list
GenericList* generic_list_create(size_t element_size)
{
    GenericList* list = (GenericList*)malloc(sizeof(GenericList));
    if (list == NULL) {
        perror("Failed to create list");
        exit(EXIT_FAILURE);
    }
    list->head = NULL;
    list->tail = NULL;
    list->element_size = element_size;
    list->size = 0;
    return list;
}

// Destroys a generic list
void generic_list_destroy(GenericList* list)
{
    Node* current = list->head;
    while (current != NULL) {
        Node* next = current->next;
        free(current->data);
        free(current);
        current = next;
    }
    free(list);
}

// Adds an element to the generic list
void* generic_list_add(GenericList* list)
{
    Node* node = (Node*)malloc(sizeof(Node));
    if (node == NULL) {
        perror("Failed to add node");
        exit(EXIT_FAILURE);
    }
    node->data = malloc(list->element_size);
    if (node->data == NULL) {
        perror("Failed to allocate data");
        exit(EXIT_FAILURE);
    }
    node->next = NULL;

    if (list->tail == NULL) {
        list->head = node;
        list->tail = node;
    } else {
        list->tail->next = node;
        list->tail = node;
    }
    list->size++;
    return node->data;
}

// Removes an element from the generic list by index
bool generic_list_remove(GenericList* list, uint32_t index)
{
    if (index >= list->size) {
        return false;
    }

    Node* current = list->head;
    Node* previous = NULL;
    for (uint32_t i = 0; i < index; i++) {
        previous = current;
        current = current->next;
    }

    if (previous == NULL) {
        list->head = current->next;
    } else {
        previous->next = current->next;
    }

    if (current == list->tail) {
        list->tail = previous;
    }

    free(current->data);
    free(current);
    list->size--;
    return true;
}

// Returns the length of the generic list
size_t generic_list_length(GenericList* list)
{
    return list->size;
}

// Creates a new iterator for the generic list
GenericListIterator* generic_list_iterator_create(GenericList* list)
{
    GenericListIterator* it = (GenericListIterator*)malloc(sizeof(GenericListIterator));
    if (it == NULL) {
        perror("Failed to create iterator");
        exit(EXIT_FAILURE);
    }
    it->current = list->head;
    it->element_size = list->element_size;
    return it;
}

// Destroys a generic list iterator
void generic_list_iterator_destroy(GenericListIterator* it)
{
    free(it);
}

// Returns the next element from the iterator
void* generic_list_iterator_next(GenericListIterator* it)
{
    if (it->current == NULL) {
        return NULL;
    }
    void* data = it->current->data;
    it->current = it->current->next;
    return data;
}
