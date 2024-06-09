#include "messages/message.h"
#include "messages/message_types.h"

void print_message(Message* message)
{

    // Prints type and length
    printf("MESSAGE{type: %s, length: %d, ", msg_get_type_name(message->header.type), message->header.payload_length);

    // Prints specific data to each message
    switch (message->header.type) {
    case MSGT_USER_CHAT: {
        UserChatMsg* chat_message = (UserChatMsg*)message;
        printf("username: %s, text: %s}\n", chat_message->user_base.username, chat_message->text);
        break;
    }

    case MSGT_FILE_HEADER: {
        FileHeaderMsg* file_info_message = (FileHeaderMsg*)message;
        printf("name: %s, size: %d}\n", file_info_message->name, file_info_message->size);
        break;
    }

    case MSGT_USER_LOGIN: {
        UserLoginMsg* login = (UserLoginMsg*)message;
        printf("type: LOGIN, username: %s}\n", login->user_base.username);
        break;
    }

    case MSGT_USER_LOGOUT: {
        printf("type: LOGOUT, ");
        break;
    }

    case MSGT_CLIENT_CONNECTED:
    case MSGT_CLIENT_ON_QUEUE: {
        printf("}\n");
        break;
    }

    case MSGT_STATUS: {
        StatusMsg* status = (StatusMsg*)message;
        printf("status: %i, text: %s}\n", status->status, status->text);
        break;
    }

    case MSGT_COMMAND: {
        CommandMsg* command = (CommandMsg*)message;
        printf("command_type: %i, arg: %s}\n", command->command_type, command->arg);
        break;
    }

    default:
        break;
    }
}
