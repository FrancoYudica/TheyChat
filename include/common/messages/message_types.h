#ifndef __MESSAGES_TYPES_H__
#define __MESSAGES_TYPES_H__

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "constants.h"

#include "task_types.h"

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

    // Command sent to server
    MSGT_TASK_REQUEST,
    MSGT_TASK_STATUS,

    // Sequence
    MSGT_SEQUENCE_START,
    MSGT_SEQUENCE_END,
    MSGT_HEAP_SEQUENCE,

    MSGT_SERVER_NOTIFICATION
};

/// @return String name of the Message
const char* msg_get_type_name(enum MessageType type);

/// @brief Empty messages that only have a type, and no data
typedef struct {
} EmptyPayload;

/// @brief Message that contains information about usual chat
typedef struct
{
    char username[MAX_USERNAME_BYTES];
    char text[MAX_CHAT_TEXT_BYTES];
    char ip[sizeof("255.255.255.255")];
    time_t time;
} UserChatPayload;

typedef struct
{
    char username[MAX_USERNAME_BYTES];

} UserLoginPayload;

typedef struct
{
    char bytes[128];
} Bytes128Payload;

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
    char text[MAX_CHAT_TEXT_BYTES];
} StatusPayload;

typedef struct
{
    uint8_t client_count;
} ConnectedClientsPayload;

typedef struct {
    time_t time;
    char text[MAX_CHAT_TEXT_BYTES];
} ServerNotificationPayload;

// TASKS ----------------------------------------------------------
typedef struct {
    bool show_ip;
    bool show_id;
} TaskUsersDada;

typedef struct
{
    /// @brief Holds the name of the file
    char filename[MAX_FILENAME_SIZE];

    /// @brief Holds the filepath of the file. This is
    /// useful when the message is received by the client
    char user_filepath[MAX_FILEPATH_SIZE];
} TaskFileUploadData;

typedef struct {
    /// @brief Holds the name of the file
    char filename[MAX_FILENAME_SIZE];
} TaskFileDownloadData;

typedef union {
    TaskUsersDada users;
    TaskFileUploadData file_upload;
    TaskFileDownloadData file_download;
} TaskData;

typedef struct
{
    enum TaskType task_type;
    TaskData data;
} TaggedTask;

typedef struct
{
    TaggedTask tagged_task;
} TaskRequestPayload;

enum TaskStatus {
    TASK_STATUS_NULL,
    TASK_STATUS_QUEUED,
    TASK_STATUS_EXECUTING,
    TASK_STATUS_COMPLETED
};

typedef struct
{
    enum TaskStatus task_status;
    TaggedTask tagged_task;
} TaskStatusPayload;

#endif