#ifndef __NET_COMMUNICATION_H__
#define __NET_COMMUNICATION_H__
#include <assert.h>
#include "messages/message.h"
#include "net_stream.h"
#include "net_error.h"


/// @brief Serializes and sends a message over a socket.
/// @param msg Pointer to the Message to send.
/// @param socketfd The file descriptor of the socket to send the message through.
ErrorCode send_message(const Message *msg, uint32_t socketfd);

/**
 * @brief Waits for a message to be received on the network stream
 *        Once the message is received, it writes the parsed message to the Message pointer.
 * 
 * @param network_stream The NetworkStream to monitor for incoming messages
 * @param sockfd The socket file descriptor to receive messages from
 * @param message Pointer to a pointer to store the parsed message received
 * @return ErrorCode The network status indicating success or error
 */
ErrorCode wait_for_message(NetworkStream* network_stream, uint32_t sockfd, Message **message);

/**
 * @brief Waits for a message of a specific type to be received on the network stream
 *        Once the message is received, it writes the parsed message to the Message pointer.
 * 
 * @param network_stream The NetworkStream to monitor for incoming messages
 * @param sockfd The socket file descriptor to receive messages from
 * @param message Pointer to a pointer to store the parsed message received
 * @param type The type of message to wait for
 * @return ErrorCode The network status indicating success or error
 */
ErrorCode wait_for_message_type(NetworkStream* network_stream, uint32_t sockfd, Message **message, uint8_t type);


#endif