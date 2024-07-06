#ifndef __COMMAND_HANDLER_DATA_H__
#define __COMMAND_HANDLER_DATA_H__
#include "command_types.h"
#include "client.h"
#include "server.h"

typedef struct
{
    Client* client;
    Server* server;
    ServerCmdRequestPayload cmd;
} CommandHandlerData;

#endif