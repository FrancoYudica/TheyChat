#include <stdlib.h>
#include "client.h"

void init_client_network(Client* client, struct sockaddr_in* address, int32_t sockfd)
{
    client->address = *address;
    client->sockfd = sockfd;

    client->ip = inet_ntoa(address->sin_addr);
}
