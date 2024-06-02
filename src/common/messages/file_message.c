#include "messages/file_message.h"
#include <malloc.h>

FileHeaderMsg* create_file_header_message(const char* name, uint32_t size)
{
    FileHeaderMsg* message = (FileHeaderMsg*)malloc(sizeof(FileHeaderMsg));

    // Initializes message data
    message->base.type = MSGT_FILE_HEADER;
    message->base.payload_length = sizeof(message->name) + sizeof(message->size);

    strcpy(message->name, name);
    message->size = size;
    return message;
}

FileContentMsg* create_file_content_message(const uint8_t* content, uint32_t size)
{
    FileContentMsg* message = (FileContentMsg*)malloc(sizeof(FileContentMsg));
    message->base.type = MSGT_FILE_CONTENT;
    message->base.payload_length = sizeof(message->content_size) + sizeof(message->binary_payload);
    message->content_size = size;
    memcpy(message->binary_payload, content, size);
    return message;
}

FileEndMsg* create_file_end_message()
{
    FileEndMsg* message = (FileEndMsg*)malloc(sizeof(FileEndMsg));
    message->base.type = MSGT_FILE_END;
    message->base.payload_length = 0;
    return message;
}
