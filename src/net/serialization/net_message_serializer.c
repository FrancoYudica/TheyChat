#include "net_message_serializer.h"
#include "message_types.h"
#include "net_serializer.h"



void ns_serialize_message(const Message *message, uint8_t *buffer, size_t *buffer_size)
{
    uint8_t* buffer_ptr = buffer;

    // Serializes message type
    ns_push_byte_array(&buffer_ptr, &message->header.type, sizeof(message->header.type));
    ns_push_long(&buffer_ptr, &message->header.payload_length, sizeof(message->header.payload_length));

    // Serializes properties for the corresponding message type
    switch (message->header.type)
    {
    case MSGT_USER_CHAT:
    {
        UserChatMsg *chat_message = (UserChatMsg*)message;
        ns_push_byte_array(&buffer_ptr, (const uint8_t*)chat_message->user_base.username, sizeof(chat_message->user_base.username));
        ns_push_byte_array(&buffer_ptr, (const uint8_t*)chat_message->text, sizeof(chat_message->text));
        break;
    }
    
    case MSGT_USER_LOGIN:
    {
        UserLoginMsg *login = (UserLoginMsg*)message;
        ns_push_byte_array(&buffer_ptr, (const uint8_t*)login->user_base.username, sizeof(login->user_base.username));
        break;
    }

    case MSGT_FILE_HEADER:
    {
        FileHeaderMsg *file_message = (FileHeaderMsg*)message;
        ns_push_byte_array(&buffer_ptr, (const uint8_t*)file_message->name, sizeof(file_message->name));
        ns_push_long(&buffer_ptr, &file_message->size, sizeof(file_message->size));
        break;
    }

    case MSGT_CLIENT_CONNECTED:
    case MSGT_CLIENT_ON_QUEUE:
    {
        Bytes128Msg *bytes_msg = (Bytes128Msg*)message;
        ns_push_byte_array(&buffer_ptr, (const uint8_t*)bytes_msg->bytes, sizeof(bytes_msg->bytes));
        break;
    }
    
    default:
        printf("Unimplemented serialization for message type %i\n", message->header.type);
        exit(EXIT_FAILURE);
        break;
    }

    // Buffer size is calculated as the difference between the pointers
    *buffer_size = buffer_ptr - buffer;
}

void ns_deserialize_message(const uint8_t* buffer, Message *message)
{
    uint8_t* buffer_ptr = (uint8_t*)buffer;

    // Pops type
    ns_pop_byte_array(&buffer_ptr, &message->header.type, sizeof(message->header.type));

    // Pops payload length
    ns_pop_long(&buffer_ptr, &message->header.payload_length, sizeof(message->header.payload_length));

    // Serializes properties for the corresponding message type
    switch (message->header.type)
    {
    case MSGT_USER_CHAT:
    {
        UserChatMsg *chat_message = (UserChatMsg*)message;
        ns_pop_byte_array(&buffer_ptr, (uint8_t*)chat_message->user_base.username, MAX_USERNAME_BYTES);
        ns_pop_byte_array(&buffer_ptr, (uint8_t*)chat_message->text, MAX_CHAT_TEXT_BYTES);
        break;
    }

    case MSGT_USER_LOGIN:
    {
        UserLoginMsg *login = (UserLoginMsg*)message;
        ns_pop_byte_array(&buffer_ptr, (uint8_t*)login->user_base.username, sizeof(login->user_base.username));
        break;
    }

    case MSGT_FILE_HEADER:
    {
        FileHeaderMsg *file_message = (FileHeaderMsg*)message;
        ns_pop_byte_array(&buffer_ptr, (uint8_t*)file_message->name, sizeof(file_message->name));
        ns_pop_long(&buffer_ptr, &file_message->size, sizeof(file_message->size));
        break;
    }

    case MSGT_CLIENT_CONNECTED:
    case MSGT_CLIENT_ON_QUEUE:
    {
        Bytes128Msg *bytes_msg = (Bytes128Msg*)message;
        ns_pop_byte_array(&buffer_ptr, (uint8_t*)bytes_msg->bytes, sizeof(bytes_msg->bytes));
        break;
    }

    default:
        printf("Unimplemented deserialization for message type %i\n", message->header.type);
        exit(EXIT_FAILURE);
        break;
    }
}