#include "messages/status_message.h"
#include <stdlib.h>

StatusMsg* create_status_msg(bool status, const char* text)
{
    StatusMsg* msg = (StatusMsg*)malloc(sizeof(StatusMsg));
    msg->header.type = MSGT_STATUS;
    msg->header.payload_length = sizeof(msg->status) + sizeof(msg->text);
    msg->status = status;

    memset(msg->text, '\0', sizeof(msg->text));

    if (text != NULL)
        strcpy(msg->text, text);

    return msg;
}

ConnectedClientsMsg* create_connected_clients_msg(uint8_t count)
{
    ConnectedClientsMsg* msg = (ConnectedClientsMsg*)malloc(sizeof(ConnectedClientsMsg));
    msg->header.type = MSGT_CONNECTED_CLIENTS;
    msg->header.payload_length = sizeof(msg->client_count);
    msg->client_count = count;
    return msg;
}
