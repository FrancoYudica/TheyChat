#ifndef __CLIENT_TYPE_H__
#define __CLIENT_TYPE_H__
#include "arpa/inet.h"

typedef struct
{
    uint32_t id;
    uint32_t sockfd;
    char* ip;
    struct sockaddr_in address;
} Client;

/// @brief Sets client attributes
void init_client_network(Client *client, struct sockaddr_in *address, uint32_t sockfd);

#endif