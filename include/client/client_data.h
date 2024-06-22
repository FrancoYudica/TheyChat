#ifndef __CLIENT_DATA_H__
#define __CLIENT_DATA_H__
#include <stdint.h>
#include "net/net_stream.h"
#include "net/net_primitives.h"

typedef struct
{
    char username[128];

    /// @brief Stores received stream from server
    NetworkStream stream;
    ConnectionContext* connection_context;

    struct
    {
        uint32_t port;
        const char* server_ip;
        bool tls_enabled;
    } connection_details;

} ClientData;

#endif