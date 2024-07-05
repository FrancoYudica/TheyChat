#ifndef __NET_CONNECTION_H__
#define __NET_CONNECTION_H__
#include "net/net_primitives.h"
#include "net/net_stream.h"

typedef struct
{
    /// @brief Connection context used to interact with
    /// network primitives
    ConnectionContext* context;

    /// @brief Network stream used to receive and store
    /// pending messages
    NetworkStream stream;

} NetworkConnection;

void init_network_connection(NetworkConnection* net_connection);
void free_network_connection(NetworkConnection* net_connection);

#endif