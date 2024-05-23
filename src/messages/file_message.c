#include "file_message.h"

void init_file_header_message(FileHeaderMsg *message, const char* name, uint32_t size)
{
    // Initializes message data
    message->base.type = MSGT_FILE_HEADER;
    message->base.payload_length = sizeof(message->name) + sizeof(message->size);

    strcpy(message->name, name);
    message->size = size;
}

void init_file_content_message(FileContentMsg* message, const uint8_t* content, uint32_t size)
{
}

void init_file_end_message(FileEndMsg* message)
{
}
