#ifndef __NET_STREAM_H__
#define __NET_STREAM_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "message.h"


typedef struct
{
    uint8_t stream[4096];
    uint32_t written_bytes;
} NetworkStream;


void init_net_stream(NetworkStream *ns){}

bool pop_message_from_stream(Message **message, NetworkStream *ns);


#endif