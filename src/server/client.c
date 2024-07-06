#include <stdlib.h>
#include "client.h"
#include "net/net_primitives.h"

void init_client_network(
    Client* client,
    ConnectionContext* status_context,
    ConnectionContext* cmd_context)
{
    static const char* default_name = "unnamed";

    // Clean initialization of attributes
    memset(client->ip, 0, sizeof(client->ip));
    memset(client->name, 0, sizeof(client->name));

    // Initializes network connections
    init_network_connection(&client->status_connection);
    init_network_connection(&client->cmd_connection);
    client->status_connection.context = status_context;
    client->cmd_connection.context = cmd_context;

    // Sets default name
    memcpy(client->name, default_name, sizeof(default_name));

    // Sets IP
    net_get_ip(status_context, client->ip, sizeof(client->ip));
}

void debug_print_client(Client* client)
{
    printf("Client(id=%i, name=%s, ip=%s) ", client->id, client->name, client->ip);
}
