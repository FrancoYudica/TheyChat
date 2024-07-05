#include "net/net_connection.h"

void init_network_connection(NetworkConnection* net_connection)
{
    net_connection->context = NULL;
    init_net_stream(&net_connection->stream);
}
void free_network_connection(NetworkConnection* net_connection)
{
    if (net_connection->context != NULL) {
        net_close(net_connection->context);
        net_connection->context = NULL;
    }
}