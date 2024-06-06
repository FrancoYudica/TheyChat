#include "messages/command_message.h"
#include <malloc.h>
CommandMsg* create_command_msg(uint8_t type, const char* arg)
{
    CommandMsg* msg = malloc(sizeof(CommandMsg));
    msg->header.type = MSGT_COMMAND;
    msg->header.payload_length = sizeof(msg->command_type) + sizeof(msg->arg);
    msg->command_type = type;

    if (arg != NULL)
        strcpy(msg->arg, arg);
    else
        msg->arg[0] = '\0';
    return msg;
}
