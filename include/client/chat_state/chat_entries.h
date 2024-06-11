#ifndef __CHAT_ENTRIES_H__
#define __CHAT_ENTRIES_H__

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "messages/message.h"

typedef struct chat_entry {
    char name[MAX_USERNAME_BYTES];
    char text[MAX_CHAT_TEXT_BYTES];
    char ip[MAX_IP_BYTES];
    uint8_t hour;
    uint8_t minute;
} ChatEntry;

// Forward declaration of ChatEntries
typedef struct chat_entries ChatEntries;

// Forward declaration of ChatEntriesIterator
typedef struct chat_entries_iterator ChatEntriesIterator;

// Function to create the ChatEntries list
ChatEntries* chat_entries_create();

// Function to add a chat entry to the ChatEntries list
void chat_entries_add(ChatEntries* list, ChatEntry entry);

// Function to get the size of the ChatEntries list
uint32_t chat_entries_get_size(const ChatEntries* list);

// Function to free the memory used by the ChatEntries list
void chat_entries_free(ChatEntries* list);

// Function to create an iterator for the ChatEntries list
ChatEntriesIterator* chat_entries_create_iterator(const ChatEntries* list, int start_index);

// Function to get the current chat entry from the iterator
bool chat_entries_iterator_get_current(const ChatEntriesIterator* iterator, const ChatEntry** entry);

// Function to move the iterator to the next chat entry
bool chat_entries_iterator_move_next(ChatEntriesIterator* iterator);

// Function to check if there are more chat entries in the iterator
bool chat_entries_iterator_has_more(const ChatEntriesIterator* iterator);

// Function to free the memory used by the ChatEntriesIterator
void chat_entries_free_iterator(ChatEntriesIterator* iterator);

#endif // CHAT_ENTRIES_H
