#ifndef __COMMAND_MESSAGE_H__
#define __COMMAND_MESSAGE_H__
#include "message.h"

#define COMMAND_ARGUMENT_SIZE 512

enum CommandType {
    CMDT_NULL = 0,
    CMDT_DISCONNECT,
    CMDT_USERS
};

typedef struct
{
    MessageHeader header;
    uint8_t command_type;
    char arg[COMMAND_ARGUMENT_SIZE];
} CommandMsg;

CommandMsg* create_command_msg(uint8_t command_type, const char* arg);

#endif