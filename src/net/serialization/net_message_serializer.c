#include "net_message_serializer.h"
#include "message_types.h"
#include "net_serializer.h"



void ns_serialize_message(const Message *message, uint8_t *buffer, size_t *buffer_size)
{
    uint8_t* buffer_ptr = buffer;

    // Serializes message type
    ns_push_byte_array(&buffer_ptr, &message->base.type, sizeof(message->base.type));
    ns_push_long(&buffer_ptr, &message->base.payload_length, sizeof(message->base.payload_length));

    // Serializes properties for the corresponding message type
    switch (message->base.type)
    {
    case MESSAGE_TYPE_USER_CHAT:
    {
        UserChatMsg *chat_message = (UserChatMsg*)message;
        ns_push_byte_array(&buffer_ptr, (const uint8_t*)chat_message->user_base.username, sizeof(chat_message->user_base.username));
        ns_push_byte_array(&buffer_ptr, (const uint8_t*)chat_message->text, sizeof(chat_message->text));
    }
        break;
    
    case MESSAGE_TYPE_FILE_INFO:
    {
        FileInfoMsg *file_message = (FileInfoMsg*)message;
        ns_push_byte_array(&buffer_ptr, (const uint8_t*)file_message->name, sizeof(file_message->name));
        ns_push_long(&buffer_ptr, &file_message->size, sizeof(file_message->size));
    }
        break;

    
    default:
        break;
    }

    // Buffer size is calculated as the difference between the pointers
    *buffer_size = buffer_ptr - buffer;
}

void ns_deserialize_message(const uint8_t* buffer, Message *message)
{
    uint8_t* buffer_ptr = (uint8_t*)buffer;

    // Pops type
    ns_pop_byte_array(&buffer_ptr, &message->base.type, sizeof(message->base.type));

    // Pops payload length
    ns_pop_long(&buffer_ptr, &message->base.payload_length, sizeof(message->base.payload_length));

    // Serializes properties for the corresponding message type
    switch (message->base.type)
    {
    case MESSAGE_TYPE_USER_CHAT:
    {
        UserChatMsg *chat_message = (UserChatMsg*)message;
        ns_pop_byte_array(&buffer_ptr, (uint8_t*)chat_message->user_base.username, MAX_USERNAME_BYTES);
        ns_pop_byte_array(&buffer_ptr, (uint8_t*)chat_message->text, MAX_CHAT_TEXT_BYTES);
    }
        break;

    case MESSAGE_TYPE_FILE_INFO:
    {
        FileInfoMsg *file_message = (FileInfoMsg*)message;
        ns_pop_byte_array(&buffer_ptr, (uint8_t*)file_message->name, sizeof(file_message->name));
        ns_pop_long(&buffer_ptr, &file_message->size, sizeof(file_message->size));
    }
        break;

    default:
        break;
    }
}