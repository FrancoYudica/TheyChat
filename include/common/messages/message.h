#ifndef __MESSAGE_H__
#define __MESSAGE_H__
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <arpa/inet.h>

enum MessageTypes {
    // User
    MSGT_USER_CHAT,
    MSGT_USER_LOGIN,
    MSGT_USER_LOGOUT,

    // File
    MSGT_FILE_HEADER,
    MSGT_FILE_CONTENT,
    MSGT_FILE_END,

    // Client
    MSGT_CLIENT_CONNECTED,
    MSGT_CLIENT_ON_QUEUE,

    // Flow control
    MSGT_STATUS
};

#define MAX_CHAT_TEXT_BYTES 256
#define MAX_USERNAME_BYTES 32

/// @brief Data shared by all messages
typedef struct
{
    uint8_t type;
    uint32_t payload_length;
} MessageHeader;

/// @brief Message basic data layout
typedef struct
{
    MessageHeader header;
} Message;

void print_message(Message* message);

#endif