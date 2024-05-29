#ifndef __CLIENT_DATA_H__
#define __CLIENT_DATA_H__
#include <stdint.h>
#include "net/net_stream.h"

typedef struct
{
    uint32_t sockfd;
    char username[128];

    /// @brief Stores received stream from server
    NetworkStream stream;
} ClientData;

#endif