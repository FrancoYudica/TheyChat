#include "file_message.h"

void init_file_info_message(FileInfoMsg *message, const char* name, uint32_t size)
{
    // Initializes message data
    message->base.type = MESSAGE_TYPE_FILE_INFO;
    message->base.payload_length = sizeof(message->name) + sizeof(message->size);

    strcpy(message->name, name);
    message->size = size;
}