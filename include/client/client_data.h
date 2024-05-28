#ifndef __CLIENT_DATA_H__
#define __CLIENT_DATA_H__
#include <stdint.h>

typedef struct
{
    uint32_t sockfd;
    char username[128];
    NetworkStream stream;
} ClientData;


#endif