#include "chat_state/chat_entries.h"

#define CHAT_ENTRIES_MAX_SIZE 128

// Define the structure for a single chat entry node (hidden from the public interface)
typedef struct chat_entry_node {
    char* name; // Name of the user
    char* text; // Chat entry text
    struct chat_entry_node* next; // Pointer to the next chat entry node
} ChatEntryNode;

// Define the structure for the ChatEntries list
struct chat_entries {
    ChatEntryNode* head; // Pointer to the first chat entry node
    ChatEntryNode* tail;
    uint32_t size; // Number of chat entries in the list
};

// Define the structure for the iterator
struct chat_entries_iterator {
    const ChatEntryNode* current; // Pointer to the current chat entry node
};

static void chat_entries_free_node(ChatEntries* list, ChatEntryNode* node)
{
    free(node->name); // Free the duplicated name string
    free(node->text); // Free the duplicated text string
    free(node);
    list->size--;
}

ChatEntries* chat_entries_create()
{
    ChatEntries* list = (ChatEntries*)malloc(sizeof(ChatEntries));
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

void chat_entries_add(ChatEntries* list, const char* name, const char* text)
{
    ChatEntryNode* new_node = (ChatEntryNode*)malloc(sizeof(ChatEntryNode));
    new_node->name = strdup(name); // Duplicate the name string
    new_node->text = strdup(text); // Duplicate the text string
    new_node->next = NULL;

    // When the first node is added
    if (list->size == 0) {
        list->head = new_node;
        list->tail = new_node;
    } else {
        // Links at the end
        list->tail->next = new_node;

        // Inserts node at the end
        list->tail = new_node;
    }
    list->size++;

    // Clears when there are more than n messages
    if (list->size > CHAT_ENTRIES_MAX_SIZE) {
        ChatEntryNode* next = list->head->next;
        chat_entries_free_node(list, list->head);
        list->head = next;
    }
}

bool chat_entries_get_by_index(const ChatEntries* list, uint32_t index, const char** name, const char** text)
{
    if (index >= list->size) {
        return false; // Index out of bounds
    }

    ChatEntryNode* current = list->head;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }

    if (current) {
        *name = current->name;
        *text = current->text;
        return true;
    }

    return false;
}

uint32_t chat_entries_get_size(const ChatEntries* list)
{
    return list->size;
}

void chat_entries_free(ChatEntries* list)
{
    ChatEntryNode* current = list->head;
    while (current != NULL) {
        ChatEntryNode* next = current->next;
        chat_entries_free_node(list, current);
        current = next;
    }

    free(list);
}

ChatEntriesIterator* chat_entries_create_iterator(const ChatEntries* list, int start_index)
{
    ChatEntriesIterator* iterator = (ChatEntriesIterator*)malloc(sizeof(ChatEntriesIterator));
    iterator->current = list->head;
    for (int i = 0; i < start_index && iterator->current != NULL; i++) {
        iterator->current = iterator->current->next;
    }
    return iterator;
}

bool chat_entries_iterator_get_current(const ChatEntriesIterator* iterator, const char** name, const char** text)
{
    if (iterator->current == NULL) {
        return false; // No current chat entry
    }

    *name = iterator->current->name;
    *text = iterator->current->text;
    return true;
}

bool chat_entries_iterator_move_next(ChatEntriesIterator* iterator)
{
    if (iterator->current == NULL) {
        return false; // No more chat entries
    }

    iterator->current = iterator->current->next;
    return (iterator->current != NULL);
}

bool chat_entries_iterator_has_more(const ChatEntriesIterator* iterator)
{
    return (iterator->current != NULL);
}

void chat_entries_free_iterator(ChatEntriesIterator* iterator)
{
    free(iterator);
}
