#include "messages/user_message.h"
#include <stdlib.h>
#include <time.h>

bool get_hours_minutes(uint8_t* hours, uint8_t* minutes)
{
    // Get the current time
    time_t current_time = time(NULL);

    // Check if getting the time failed
    if (current_time == ((time_t)-1)) {
        return false;
    }

    // Convert the current time to local time
    struct tm* local_time = localtime(&current_time);

    // Check if converting the time failed
    if (local_time == NULL) {
        return false;
    }

    // Get the hours and minutes
    *hours = local_time->tm_hour;
    *minutes = local_time->tm_min;
    return true;
}

UserChatMsg* create_user_chat_msg(const char* text, const char* username)
{
    UserChatMsg* message = (UserChatMsg*)malloc(sizeof(UserChatMsg));
    message->user_base.header.type = MSGT_USER_CHAT;
    message->user_base.header.payload_length = sizeof(message->user_base.username) + sizeof(message->text) + sizeof(message->ip) + sizeof(message->hours) + sizeof(message->minutes);
    strcpy(message->text, text);
    strcpy(message->user_base.username, username);
    message->hours = 0;
    message->minutes = 0;
    get_hours_minutes(&message->hours, &message->minutes);
    return message;
}

UserLoginMsg* create_user_login_msg(const char* username)
{
    UserLoginMsg* message = (UserLoginMsg*)malloc(sizeof(UserLoginMsg));
    message->user_base.header.type = MSGT_USER_LOGIN;
    message->user_base.header.payload_length = sizeof(message->user_base.username);
    strcpy(message->user_base.username, username);
    return message;
}
