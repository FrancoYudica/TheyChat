#ifndef __NET_COMMUNICATION_H__
#define __NET_COMMUNICATION_H__
#include "message.h"

void send_message_to_socketfd(const Message *msg, uint32_t socketfd);

#endif