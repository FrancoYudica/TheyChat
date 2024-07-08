#ifndef __CLIENT_H__
#define __CLIENT_H__
#include <stdint.h>
#include "constants.h"
#include "net/net_connection.h"
typedef struct
{
    char username[128];

    /// @brief NetworkConnection used to exchange status messages
    NetworkConnection status_connection;

    /// @brief NetworkConnection used to communicate with the server
    /// only for tasks, such as file transferring, which happens
    /// asynchronously of the status messages, such as user chat
    NetworkConnection task_connection;

    struct
    {
        uint32_t status_port;
        uint32_t task_port;
        char server_ip[MAX_IP_BYTES];
        bool tls_enabled;
    } connection_details;

} Client;

/// @brief Singleton client getter
Client* get_client();

#endif