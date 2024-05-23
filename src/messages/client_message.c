#include "client_message.h"
#include <stdlib.h>

Bytes128Msg* create_client_connected()
{
    Bytes128Msg *msg = malloc(sizeof(Bytes128Msg));
    static const char text[] = "Welcome to TheyChat!";

    msg->header.type = MSGT_CLIENT_CONNECTED;
    msg->header.payload_length = 128;

    strcpy(msg->bytes, text);
    return msg;
}

Bytes128Msg* create_client_on_queue()
{
    Bytes128Msg *msg = malloc(sizeof(Bytes128Msg));
    static const char text[] = "Connected to server. In client queue...";

    msg->header.type = MSGT_CLIENT_ON_QUEUE;
    msg->header.payload_length = 128;

    strcpy(msg->bytes, text);
    return msg;
}
