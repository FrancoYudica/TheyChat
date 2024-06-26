#ifndef __NET_COMMUNICATION_H__
#define __NET_COMMUNICATION_H__

#include <assert.h>
#include "messages/message.h"
#include "net/net_stream.h"
#include "they_chat_error.h"

// Forward declaration of the ConnectionContext structure
typedef struct ConnectionContext ConnectionContext;

/**
 * @brief Serializes and sends a message over a socket.
 *
 * @param msg Pointer to the Message to send.
 * @param connection_context Pointer to the ConnectionContext structure containing the socket.
 * @return Error* indicating success or failure.
 */
Error* send_message(const Message* msg, ConnectionContext* connection_context);

/**
 * @brief Waits for a message to be received on the network stream.
 *        Once the message is received, it writes the parsed message to the Message pointer.
 *        Note that the double pointer is necessary since the message type is unknown.
 *
 * @param network_stream The NetworkStream to monitor for incoming messages.
 * @param connection_context Pointer to the ConnectionContext structure containing the socket.
 * @param message Pointer to store the parsed message received.
 * @return Error* The network status indicating success or error.
 */
Error* wait_for_message(NetworkStream* network_stream, ConnectionContext* connection_context, Message* message);

/**
 * @brief Waits for a message of a specific type to be received on the network stream.
 *        Once the message is received, it writes the parsed message to the Message pointer.
 *
 * @param network_stream The NetworkStream to monitor for incoming messages.
 * @param connection_context Pointer to the ConnectionContext structure containing the socket.
 * @param message Pointer to store the parsed message received.
 * @param type The type of message to wait for.
 * @return Error* The network status indicating success or error.
 */
Error* wait_for_message_type(NetworkStream* network_stream, ConnectionContext* connection_context, Message* message, uint8_t type);

#endif // __NET_COMMUNICATION_H__
