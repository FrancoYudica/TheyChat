#include <stdlib.h>
#include "client.h"

void init_client_network(Client* client, ConnectionContext context)
{
    client->connection_context = context;
    client->ip = inet_ntoa(client->connection_context.addr.sin_addr);
}
