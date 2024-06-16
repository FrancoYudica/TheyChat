#ifndef __SEQUENCE_MSG_H__
#define __SEQUENCE_MSG_H__
#include "message.h"
#include <stdint.h>

/// @brief Message that stores a payload of varying size
typedef struct
{
    MessageHeader header;
    uint8_t* payload;
} HeapSequenceMsg;

/// @brief Message that tells that a sequence is about to start
typedef EmptyMessage SequenceStartMsg;

/// @brief Message that tells that a sequence ended
typedef EmptyMessage SequenceEndMsg;

SequenceStartMsg* create_seq_start_msg();

SequenceEndMsg* create_seq_end_msg();

HeapSequenceMsg* create_heap_seq_msg(const uint8_t* payload, uint32_t payload_size);

#endif