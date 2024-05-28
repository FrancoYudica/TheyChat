#ifndef __STATE_HANDLER_UTILS_H__
#define __STATE_HANDLER_UTILS_H__
#include "messages/message_types.h"
#include "common/application_states.h"
#include "net/net_communication.h"
#include "net/net_error.h"
#include "server/server.h"
#include "server/client.h"

/// @brief Holds all the relevant information
/// for state handler, in order to handle client
typedef struct
{
    Server *server;
    Client *client;
} ServerStateData;

#endif