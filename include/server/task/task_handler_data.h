#ifndef __TASK_HANDLER_DATA_H__
#define __TASK_HANDLER_DATA_H__
#include "client.h"
#include "server.h"

typedef struct
{
    Client* client;
    Server* server;
    TaskRequestPayload task_request;
} TaskHandlerData;

#endif