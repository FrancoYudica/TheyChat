#ifndef __CLIENT_TYPE_H__
#define __CLIENT_TYPE_H__
#include "arpa/inet.h"
#include "net/net_stream.h"
#include "net/net_primitives.h"

typedef struct
{
    uint32_t id;
    char ip[sizeof("255.255.255.255") + 1];
    char name[MAX_USERNAME_BYTES];
    ConnectionContext* connection_context;
    NetworkStream stream;
} Client;

/// @brief Sets client attributes
void init_client_network(Client* client, ConnectionContext* context);

void debug_print_client(Client* client);

#endif