#ifndef __MESSAGE_H__
#define __MESSAGE_H__
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>


enum MessageTypes
{
    MESSAGE_TYPE_USER_CHAT,
    MESSAGE_TYPE_USER_LOGIN,
    MESSAGE_TYPE_USER_LOGOUT,
    MESSAGE_TYPE_FILE_INFO
};

#define MAX_CHAT_TEXT_BYTES 256
#define MAX_USERNAME_BYTES 32

/// @brief Data shared by all messages
typedef struct
{
    uint8_t type;
    uint32_t payload_length;
} BaseMessage;

/// @brief Message basic data layout
typedef struct 
{
    BaseMessage base;
} Message;



void print_message(Message *message);

#endif