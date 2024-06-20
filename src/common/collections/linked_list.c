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

// LinkedList structure
typedef struct LinkedList {
    Node* head;
    Node* tail;
    size_t element_size;
    size_t size;
} LinkedList;

// LinkedListIterator structure
typedef struct LinkedListIterator {
    Node* current;
    size_t element_size;
} LinkedListIterator;

// Creates a new generic list
LinkedList* linked_list_create(size_t element_size)
{
    LinkedList* list = (LinkedList*)malloc(sizeof(LinkedList));
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
void linked_list_destroy(LinkedList* list)
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
void* linked_list_add(LinkedList* list)
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
bool linked_list_remove(LinkedList* list, uint32_t index)
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
size_t linked_list_length(LinkedList* list)
{
    return list->size;
}

// Creates a new iterator for the generic list
LinkedListIterator* linked_list_iterator_create(LinkedList* list)
{
    LinkedListIterator* it = (LinkedListIterator*)malloc(sizeof(LinkedListIterator));
    if (it == NULL) {
        perror("Failed to create iterator");
        exit(EXIT_FAILURE);
    }
    it->current = list->head;
    it->element_size = list->element_size;
    return it;
}

// Destroys a generic list iterator
void linked_list_iterator_destroy(LinkedListIterator* it)
{
    free(it);
}

// Returns the next element from the iterator
void* linked_list_iterator_next(LinkedListIterator* it)
{
    if (it->current == NULL) {
        return NULL;
    }
    void* data = it->current->data;
    it->current = it->current->next;
    return data;
}
