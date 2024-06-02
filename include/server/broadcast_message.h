#ifndef __BROADCAST_MESSAGE_H__
#define __BROADCAST_MESSAGE_H__
#include "messages/message.h"
#include "server_state_data.h"

/// @brief Sends message to all clients
void send_broadcast(const Message* msg, ServerStateData* state_data);

/// @brief Sends message to all clients, except for the specified client
void send_broadcast_exclude(const Message* msg, ServerStateData* state_data, Client* client);

#endif