#include "user_message.h"

void init_user_chat_msg(UserChatMsg *message, const char *text, const char *username)
{
    // Initializes message data
    message->user_base.base.type = MESSAGE_TYPE_USER_CHAT;
    message->user_base.base.payload_length = sizeof(message->user_base.username) + sizeof(message->text);

    strcpy(message->text, text);
    strcpy(message->user_base.username, username);
}

void init_user_login_msg(UserLoginMsg *message, const char *username)
{
    message->user_base.base.type = MESSAGE_TYPE_USER_LOGIN;
    message->user_base.base.payload_length = sizeof(message->user_base.username);
    strcpy(message->user_base.username, username);
}
