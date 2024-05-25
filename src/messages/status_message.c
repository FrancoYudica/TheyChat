#include "status_message.h"

StatusMsg* create_status_msg(bool status, const char *text)
{
    StatusMsg *msg = (StatusMsg*)malloc(sizeof(StatusMsg));
    msg->header.type = MSGT_STATUS;
    msg->header.payload_length = sizeof(msg->status) + sizeof(msg->text);
    msg->status = status;

    memset(msg->text, '\0', sizeof(msg->text));
    
    if (text != NULL)
        strcpy(msg->text, text);

    return msg;
}
