#ifndef __CLIENT_TYPE_H__
#define __CLIENT_TYPE_H__
#include "arpa/inet.h"
#include "net/net_stream.h"

typedef struct
{
    uint32_t id;
    uint32_t sockfd;
    char* ip;
    char name[MAX_USERNAME_BYTES];
    struct sockaddr_in address;
    NetworkStream stream;
} Client;

/// @brief Sets client attributes
void init_client_network(Client* client, struct sockaddr_in* address, int32_t sockfd);

#endif