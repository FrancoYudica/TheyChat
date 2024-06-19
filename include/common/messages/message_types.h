#ifndef __MESSAGES_TYPES_H__
#define __MESSAGES_TYPES_H__

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

enum MessageType {
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

    // Status messages
    MSGT_STATUS,
    MSGT_CONNECTED_CLIENTS,

    // Command
    MSGT_COMMAND,

    // Sequence
    MSGT_SEQUENCE_START,
    MSGT_SEQUENCE_END,
    MSGT_HEAP_SEQUENCE
};

/// Amount of actual content bytes sent
#define FILE_CONTENT_SIZE 512
#define COMMAND_ARGUMENT_SIZE 512
#define MAX_CHAT_TEXT_BYTES 128
#define MAX_USERNAME_BYTES 32
#define MAX_IP_BYTES sizeof("255.255.255.255")
#define STATUS_MSG_SUCCESS 1
#define STATUS_MSG_FAILURE 0

/// @return String name of the Message
const char* msg_get_type_name(uint8_t type);

/// @brief Empty messages that only have a type, and no data
typedef struct {
} EmptyPayload;

/// @brief Message that contains information about usual chat
typedef struct
{
    char username[MAX_USERNAME_BYTES];
    char text[MAX_CHAT_TEXT_BYTES];
    char ip[sizeof("255.255.255.255")];
    uint32_t time;
} UserChatPayload;

typedef struct
{
    char username[MAX_USERNAME_BYTES];

} UserLoginPayload;

typedef struct
{
    char bytes[128];
} Bytes128Payload;

enum CommandType {
    CMDT_NULL = 0,
    CMDT_DISCONNECT,
    CMDT_USERS
};

typedef struct
{
    uint8_t command_type;
    char arg[COMMAND_ARGUMENT_SIZE];
} CommandPayload;

/// @brief Stores descriptive data of the file
typedef struct
{
    /// @brief Name of the file
    char name[64];

    /// @brief Size of the file in bytes
    uint32_t size;

} FileHeaderPayload;

typedef struct
{
    /// @brief Size in bytes of the actual content. This is necessary
    /// since the last FileContentMsg won't fill the entire binary_payload buffer
    uint32_t content_size;
    uint8_t binary_payload[FILE_CONTENT_SIZE];
} FileContentPayload;

typedef EmptyPayload FileEndPayload;

/// @brief Message that stores a payload of varying size
typedef struct
{
    uint8_t* payload;
} HeapSequencePayload;

/// @brief Message that tells that a sequence is about to start
typedef EmptyPayload SequenceStartPayload;

/// @brief Message that tells that a sequence ended
typedef EmptyPayload SequenceEndPayload;

typedef struct
{
    // Boolean flag
    bool status;

    // Contains text explaining the status
    char text[64];
} StatusPayload;

typedef struct
{
    uint8_t client_count;
} ConnectedClientsPayload;

typedef Bytes128Payload ServerNotification;

#endif