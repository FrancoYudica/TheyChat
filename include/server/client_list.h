#ifndef __DS_CLIENT_LIST_H__
#define __DS_CLIENT_LIST_H__
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "client.h"

// Forward declaration of the ClientList structure to hide implementation details.
typedef struct ClientList ClientList;

/**
 * @brief Creates a new client list.
 *
 * @return A pointer to the newly created client list.
 */
ClientList* client_list_create();

/**
 * @brief Destroys a client list and frees all associated memory, including clients.
 *
 * @param client_list The client list to destroy.
 */
void client_list_destroy(ClientList* client_list);

/**
 * @brief Adds a client to the client list. Adding means allocating a copy of the client
 *        in the linked list.
 *
 * @param client_list The client list to which the client will be added.
 */
Client* client_list_add(ClientList* client_list);

/**
 * @brief Removes and frees client from the client list.
 *
 * @param client_list The client list from which to remove the client.
 * @param client_id The ID of the client to remove.
 *
 * @return true if the client was successfully removed, false otherwise.
 */
bool client_list_remove(ClientList* client_list, uint32_t client_id);

/**
 * @brief Finds a client in the client list by its ID.
 *
 * @param client_list The client list in which to search for the client.
 * @param client_id The ID of the client to find.
 *
 * @return A pointer to the found client, or NULL if the client was not found.
 */
Client* client_list_find_by_id(ClientList* client_list, uint32_t client_id);

/**
 * @brief Finds a client in the client list by its name.
 *
 * @param client_list The client list in which to search for the client.
 * @param client_name The name of the client to find.
 *
 * @return A pointer to the found client, or NULL if the client was not found.
 */
Client* client_list_find_by_name(ClientList* client_list, const char* name);

/**
 * @brief Returns the length of the client list.
 *
 * @param client_list The client list.
 *
 * @return The number of clients in the client list.
 */
size_t client_list_length(ClientList* client_list);

/**
 * @brief Advances and returns client list iterator
 *
 * @param client_list The client list.
 *
 * @return Next client in the iterator.
 */
Client* client_list_interator_next(ClientList* client_list);

/// @brief Sets the client iterator pointer to the front of the list
/// @param client_list
void client_list_interator_rewind(ClientList* client_list);

#endif // __DS_CLIENT_LIST_H__
