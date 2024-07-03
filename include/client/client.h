#ifndef __CLIENT_H__
#define __CLIENT_H__
#include <stdint.h>
#include "constants.h"
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
        char server_ip[MAX_IP_BYTES];
        bool tls_enabled;
    } connection_details;

} Client;

/// @brief Singleton client getter
Client* get_client();

#endif