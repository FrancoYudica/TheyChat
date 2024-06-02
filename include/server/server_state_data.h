#ifndef __SERVER_STATE_DATA_H__
#define __SERVER_STATE_DATA_H__
#include <pthread.h>
#include <stddef.h>
#include "server.h"
#include "client.h"

/// @brief Holds all the relevant information
/// for state handler, in order to handle client
typedef struct
{
    Server* server;
    Client* client;

    /// @brief Mutex used exclusively to block other
    /// threads when sending messages in broadcast
    pthread_mutex_t broadcast_mutex;
} ServerStateData;

ServerStateData create_server_data(Server* server, Client* client);

#endif