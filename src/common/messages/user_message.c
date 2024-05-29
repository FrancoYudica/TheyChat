#include "messages/user_message.h"
#include <stdlib.h>


UserChatMsg* create_user_chat_msg(const char* text, const char* username)
{
    UserChatMsg *message = (UserChatMsg*)malloc(sizeof(UserChatMsg));
    message->user_base.header.type = MSGT_USER_CHAT;
    message->user_base.header.payload_length = sizeof(message->user_base.username) + sizeof(message->text);
    strcpy(message->text, text);
    strcpy(message->user_base.username, username);
    return message;
}

UserLoginMsg* create_user_login_msg(const char* username)
{
    UserLoginMsg *message = (UserLoginMsg*)malloc(sizeof(UserLoginMsg));
    message->user_base.header.type = MSGT_USER_LOGIN;
    message->user_base.header.payload_length = sizeof(message->user_base.username);
    strcpy(message->user_base.username, username);
    return message;
}
