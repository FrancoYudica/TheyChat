#ifndef __SERVER_H__
#define __SERVER_H__
#include "client_list.h"
#include "pthread.h"

typedef struct
{
    ClientList* client_list;
    uint32_t port;
    uint32_t sockfd;
    pthread_mutex_t client_list_mutex;
} Server;

#endif