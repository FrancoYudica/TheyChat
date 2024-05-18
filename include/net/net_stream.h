#ifndef __NET_STREAM_H__
#define __NET_STREAM_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "message.h"


typedef struct
{
    /// @brief Buffer where unprocessed bytes are stored
    uint8_t stream[4096];

    /// @brief Amount of unprocessed bytes
    uint32_t written_bytes;
} NetworkStream;


void init_net_stream(NetworkStream *ns);

bool stream_pop_message(Message **message, NetworkStream *ns);

bool stream_pop_message_by_type(uint8_t expected_type, Message **message, NetworkStream *ns);

#endif