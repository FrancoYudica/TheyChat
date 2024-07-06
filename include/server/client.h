#ifndef __CLIENT_TYPE_H__
#define __CLIENT_TYPE_H__
#include "net/net_connection.h"
#include "net/net_primitives.h"
#include "application_states.h"

typedef struct
{
    uint32_t id;
    char ip[sizeof("255.255.255.255") + 1];
    char name[MAX_USERNAME_BYTES];

    NetworkConnection status_connection;
    NetworkConnection cmd_connection;

    AppState current_state;
} Client;

/// @brief Sets client attributes
void init_client_network(
    Client* client,
    ConnectionContext* status_context,
    ConnectionContext* cmd_context);

void debug_print_client(Client* client);

#endif