#ifndef __CLIENT_DATA_H__
#define __CLIENT_DATA_H__
#include <stdint.h>
#include "net/net_stream.h"
#include "net/network.h"

typedef struct
{
    char username[128];

    /// @brief Stores received stream from server
    NetworkStream stream;
    ConnectionContext* connection_context;
} ClientData;

#endif