#include "client_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client.h"
#include "collections/linked_list.h"

struct ClientList {
    GenericList* list;
    uint32_t next_client_id;
    GenericListIterator* it;
};

ClientList* client_list_create()
{
    ClientList* client_list = (ClientList*)malloc(sizeof(ClientList));
    if (client_list == NULL) {
        perror("Failed to initialize list");
        exit(EXIT_FAILURE);
    }
    client_list->list = generic_list_create(sizeof(Client));
    if (client_list->list == NULL) {
        perror("Failed to initialize list");
        exit(EXIT_FAILURE);
    }

    client_list->next_client_id = 0;
    client_list->it = generic_list_iterator_create(client_list->list);

    return client_list;
}

void client_list_destroy(ClientList* client_list)
{
    generic_list_destroy(client_list->list);
    free(client_list);
}

Client* client_list_add(ClientList* client_list)
{
    Client* client = (Client*)generic_list_add(client_list->list);
    client->id = client_list->next_client_id++;
    return client;
}

bool client_list_remove(ClientList* client_list, uint32_t client_id)
{
    // Create an iterator for the generic list
    GenericListIterator* it = generic_list_iterator_create(client_list->list);
    Client* client;
    bool found = false;
    uint32_t index = 0;

    // Iterate through the list to find the client
    while ((client = (Client*)generic_list_iterator_next(it)) != NULL) {
        if (client->id == client_id) {
            found = true;
            break;
        }
        index++;
    }

    // Destroy the iterator after use
    generic_list_iterator_destroy(it);

    // If the client was not found, return false
    if (!found)
        return false;

    // Remove the client from the list at the found index
    return generic_list_remove(client_list->list, index);
}

Client* client_list_find_by_id(ClientList* client_list, uint32_t client_id)
{
    GenericListIterator* it = generic_list_iterator_create(client_list->list);
    Client* client = NULL;

    while ((client = (Client*)generic_list_iterator_next(it)) != NULL) {
        if (client->id == client_id) {
            break;
        }
    }

    generic_list_iterator_destroy(it);

    return client; // Will be NULL if not found
}

Client* client_list_find_by_name(ClientList* client_list, const char* name)
{
    GenericListIterator* it = generic_list_iterator_create(client_list->list);
    Client* client = NULL;

    while ((client = (Client*)generic_list_iterator_next(it)) != NULL) {
        if (strcmp(client->name, name) == 0) {
            break;
        }
    }

    generic_list_iterator_destroy(it);

    return client; // Will be NULL if not found
}

size_t client_list_length(ClientList* client_list)
{
    return generic_list_length(client_list->list);
}

Client* client_list_interator_next(ClientList* client_list)
{
    return (Client*)generic_list_iterator_next(client_list->it);
}

void client_list_interator_rewind(ClientList* client_list)
{
    generic_list_iterator_destroy(client_list->it);
    client_list->it = generic_list_iterator_create(client_list->list);
}
