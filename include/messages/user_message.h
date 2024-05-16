#include "../message.h"

// Base struct for all user messages
typedef struct
{
    BaseMessage base;
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


void init_user_chat_msg(UserChatMsg *message, const char* text, const char* username);

void init_user_login_msg(UserLoginMsg *message, const char *username);
