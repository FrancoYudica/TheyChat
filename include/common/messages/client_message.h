// This file contains messages that are unique to the
#ifndef __CLIENT_MESSAGE_H__
#define __CLIENT_MESSAGE_H__
#include "message.h"

typedef struct
{
    MessageHeader header;
    char bytes[128];
} Bytes128Msg;

typedef struct
{
    MessageHeader header;
    char bytes[512];
} Bytes512Msg;

Bytes128Msg* create_client_connected();
Bytes128Msg* create_client_on_queue();

#endif