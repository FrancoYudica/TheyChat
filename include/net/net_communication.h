#ifndef __NET_COMMUNICATION_H__
#define __NET_COMMUNICATION_H__
#include "message.h"
#include "net_stream.h"

/// @brief Serializes and sends a message over a socket.
/// @param msg Pointer to the Message to send.
/// @param socketfd The file descriptor of the socket to send the message through.
void send_message(const Message *msg, uint32_t socketfd);

/// @brief Receives bytes from a network socket and writes them into a network stream.
/// @param network_stream Pointer to the NetworkStream to write the received bytes into.
/// @param sockfd The file descriptor of the socket to receive bytes from.
void receive(NetworkStream* network_stream, uint32_t sockfd);

#endif