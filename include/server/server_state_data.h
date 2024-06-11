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
} ServerStateData;

ServerStateData create_server_data(Server* server, Client* client);

#endif