#ifndef __CLIENT_HANDLER_H__
#define __CLIENT_HANDLER_H__
#include "server.h"

typedef struct
{
    Server* server;
    Client* client;
} ClientHandlerData;

/// @brief Handles all client interactions
void handle_client_task(ClientHandlerData* client_handler_data);

#endif