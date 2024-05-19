#include "message.h"
#include "message_types.h"

void print_message(Message *message)
{

    // Prints type and length
    printf("MESSAGE{type: %s, length: %d, ", msg_get_type_name(message->base.type),  message->base.payload_length);

    // Prints specific data to each message
    switch (message->base.type)
    {
    case MESSAGE_TYPE_USER_CHAT:
    {
        UserChatMsg *chat_message = (UserChatMsg*)message;
        printf("username: %s, text: %s}\n", chat_message->user_base.username, chat_message->text);
        break;
    }
    
    case MESSAGE_TYPE_FILE_INFO:
    {
        FileInfoMsg *file_info_message = (FileInfoMsg*)message;
        printf("name: %s, size: %d}\n", file_info_message->name, file_info_message->size);
        break;
    }

    case MESSAGE_TYPE_USER_LOGIN:
    {
        printf("type: LOGIN, ");
        break;
    }

    case MESSAGE_TYPE_USER_LOGOUT:
    {
        printf("type: LOGOUT, ");
        break;
    }

    default:
        break;
    }
}

