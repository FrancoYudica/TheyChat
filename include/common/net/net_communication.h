#ifndef __NET_COMMUNICATION_H__
#define __NET_COMMUNICATION_H__

#include <assert.h>
#include "messages/message.h"
#include "net/net_connection.h"
#include "they_chat_error.h"

// Forward declaration of the ConnectionContext structure
typedef struct ConnectionContext ConnectionContext;

/// @brief Serializes and sends a message over a socket.
Error* send_message(const Message* msg, NetworkConnection* net_connection);

/// @brief Waits for a message to be received on the network stream.
///        Once the message is received, it writes the parsed message to the Message pointer.
Error* wait_for_message(NetworkConnection* connection, Message* message);

/// @brief Waits for a message of a specific type to be received on the network stream.
///        Once the message is received, it writes the parsed message to the Message pointer.
Error* wait_for_message_type(NetworkConnection* connection, Message* message, uint8_t type);

#endif // __NET_COMMUNICATION_H__
