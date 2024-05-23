#ifndef __SERVER_H__
#define __SERVER_H__
#include "client_list.h"


typedef struct
{
    ClientList *client_list;
    uint32_t port;
    uint32_t sockfd;
} Server;

#endif