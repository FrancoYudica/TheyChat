#ifndef __STATUS_MESSAGE_H__
#define __STATUS_MESSAGE_H__
#include "message.h"

#define STATUS_MSG_SUCCESS 1
#define STATUS_MSG_FAILURE 0

typedef struct
{
    MessageHeader header;

    // Boolean flag
    bool status;

    // Contains text explaining the status
    char text[64];
} StatusMsg;

StatusMsg* create_status_msg(bool status, const char *text);

#endif