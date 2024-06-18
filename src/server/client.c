#include <stdlib.h>
#include "client.h"
#include "net/net_primitives.h"

void init_client_network(Client* client, ConnectionContext* context)
{

    static const char* default_name = "unnamed";

    // Clean initialization of attributes
    memset(client->ip, 0, sizeof(client->ip));
    memset(client->name, 0, sizeof(client->name));

    // Sets context
    client->connection_context = context;

    // Sets default name
    memcpy(client->name, default_name, sizeof(default_name));

    // Sets IP
    net_get_ip(context, client->ip, sizeof(client->ip));

    init_net_stream(&client->stream);
}

void debug_print_client(Client* client)
{
    printf("Client(id=%i, name=%s, ip=%s) ", client->id, client->name, client->ip);
}
