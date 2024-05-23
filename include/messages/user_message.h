#include "message.h"

// Base struct for all user messages
typedef struct
{
    MessageHeader header;
    char username[MAX_USERNAME_BYTES];
} UserBaseMsg;


/// @brief Message that contains information about usual chat
typedef struct 
{
    UserBaseMsg user_base;
    char text[MAX_CHAT_TEXT_BYTES];
} UserChatMsg;


typedef struct
{
    UserBaseMsg user_base;

} UserLoginMsg;


UserChatMsg* create_user_chat_msg(const char* text, const char* username);

UserLoginMsg* create_user_login_msg(const char *username);
