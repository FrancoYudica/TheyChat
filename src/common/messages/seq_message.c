#include <malloc.h>
#include "messages/seq_message.h"

SequenceStartMsg* create_seq_start_msg()
{
    SequenceStartMsg* message = (SequenceStartMsg*)malloc(sizeof(SequenceStartMsg));
    message->header.payload_length = 0;
    message->header.type = MSGT_SEQUENCE_START;
    return message;
}

SequenceEndMsg* create_seq_end_msg()
{
    SequenceEndMsg* message = (SequenceEndMsg*)malloc(sizeof(SequenceEndMsg));
    message->header.payload_length = 0;
    message->header.type = MSGT_SEQUENCE_END;
    return message;
}

HeapSequenceMsg* create_heap_seq_msg(const uint8_t* payload, uint32_t payload_size)
{
    HeapSequenceMsg* message = (HeapSequenceMsg*)malloc(sizeof(HeapSequenceMsg));
    message->header.payload_length = payload_size;
    message->header.type = MSGT_HEAP_SEQUENCE;
    message->payload = malloc(payload_size);
    memcpy(message->payload, payload, payload_size);
    return message;
}
