#ifndef __NET_COMMUNICATION_H__
#define __NET_COMMUNICATION_H__

#include <assert.h>
#include "messages/message.h"
#include "net/net_stream.h"
#include "net/net_error.h"
#include "net/net_primitives.h"

/**
 * @brief Serializes and sends a message over a socket.
 *
 * @param msg Pointer to the Message to send.
 * @param connection_context Pointer to the ConnectionContext structure containing the socket.
 * @return ErrorCode Error code indicating success or failure.
 */
ErrorCode send_message(const Message* msg, ConnectionContext* connection_context);

/**
 * @brief Waits for a message to be received on the network stream.
 *        Once the message is received, it writes the parsed message to the Message pointer.
 *        Note that the double pointer is necessary since the message type is unknown.
 *
 * @param network_stream The NetworkStream to monitor for incoming messages.
 * @param connection_context Pointer to the ConnectionContext structure containing the socket.
 * @param message Pointer to a pointer to store the parsed message received.
 * @return ErrorCode The network status indicating success or error.
 */
ErrorCode wait_for_message(NetworkStream* network_stream, ConnectionContext* connection_context, Message** message);

/**
 * @brief Waits for a message of a specific type to be received on the network stream.
 *        Once the message is received, it writes the parsed message to the Message pointer.
 *
 * @param network_stream The NetworkStream to monitor for incoming messages.
 * @param connection_context Pointer to the ConnectionContext structure containing the socket.
 * @param message Pointer to a pointer to store the parsed message received.
 * @param type The type of message to wait for.
 * @return ErrorCode The network status indicating success or error.
 */
ErrorCode wait_for_message_type(NetworkStream* network_stream, ConnectionContext* connection_context, Message** message, uint8_t type);

#endif // __NET_COMMUNICATION_H__
