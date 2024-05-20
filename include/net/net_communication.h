#ifndef __NET_COMMUNICATION_H__
#define __NET_COMMUNICATION_H__
#include "message.h"
#include "net_stream.h"

#define NET_SUCCESS 0
#define NET_ERROR_PEER_DISCONNECTED -1
#define NET_ERROR_RECEIVE_FAILED -2
#define NET_ERROR_INVALID_TYPE -3
#define NET_ERROR_SEND_FAILED -4
#define IS_NET_ERROR(X) (X < 0)

/// @brief Type used to retrieve status after network operations
typedef int8_t net_status_t;

/// @brief Serializes and sends a message over a socket.
/// @param msg Pointer to the Message to send.
/// @param socketfd The file descriptor of the socket to send the message through.
net_status_t send_message(const Message *msg, uint32_t socketfd);

/**
 * @brief Waits for a message to be received on the network stream
 *        Once the message is received, it writes the parsed message to the Message pointer.
 * 
 * @param network_stream The NetworkStream to monitor for incoming messages
 * @param sockfd The socket file descriptor to receive messages from
 * @param message Pointer to a pointer to store the parsed message received
 * @return net_status_t The network status indicating success or error
 */
net_status_t wait_for_message(NetworkStream* network_stream, uint32_t sockfd, Message **message);

/**
 * @brief Waits for a message of a specific type to be received on the network stream
 *        Once the message is received, it writes the parsed message to the Message pointer.
 * 
 * @param network_stream The NetworkStream to monitor for incoming messages
 * @param sockfd The socket file descriptor to receive messages from
 * @param message Pointer to a pointer to store the parsed message received
 * @param type The type of message to wait for
 * @return net_status_t The network status indicating success or error
 */
net_status_t wait_for_message_type(NetworkStream* network_stream, uint32_t sockfd, Message **message, uint8_t type);


#endif