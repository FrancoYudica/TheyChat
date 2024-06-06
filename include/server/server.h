#ifndef __SERVER_H__
#define __SERVER_H__
#include "client_list.h"
#include "pthread.h"
#include "net/net_primitives.h"
typedef struct
{
    ClientList* client_list;
    uint32_t port;
    pthread_mutex_t client_list_mutex;
    ConnectionContext* context;
} Server;

#endif