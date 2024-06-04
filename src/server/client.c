#include <stdlib.h>
#include "client.h"

void init_client_network(Client* client, ConnectionContext context)
{
    client->connection_context = context;
    client->address = client->connection_context.addr;
    client->sockfd = client->connection_context.socketfd;

    client->ip = inet_ntoa(client->connection_context.addr.sin_addr);
}
