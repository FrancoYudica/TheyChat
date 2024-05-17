#ifndef __NET_COMMUNICATION_H__
#define __NET_COMMUNICATION_H__
#include "message.h"
#include "net_stream.h"

void send_message_to_socketfd(const Message *msg, uint32_t socketfd);

void receive_message_from_socketfd(Message **message, uint32_t sockfd);

void receive(NetworkStream* network_stream, uint32_t sockfd);

#endif