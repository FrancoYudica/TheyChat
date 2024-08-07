#ifndef __MESSAGE_H__
#define __MESSAGE_H__
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>

#include "messages/message_types.h"

/// @brief Message basic data layout
typedef struct
{
    enum MessageType type;

    /// @brief Length in bytes of the message payload.
    /// stores the total size of all the fields of the message
    /// added, without considering padding.
    uint32_t net_payload_length;

    union MessagePayload {
        UserChatPayload user_chat;
        UserLoginPayload user_login;
        Bytes128Payload bytes_128;

        TaskRequestPayload task_request;
        TaskStatusPayload task_status;

        FileHeaderPayload file_header;
        FileContentPayload file_content;
        FileEndPayload file_end;

        SequenceStartPayload sequence_start;
        SequenceEndPayload sequence_end;
        HeapSequencePayload heap_sequence;

        StatusPayload status;
        ConnectedClientsPayload connected_clients;
        ServerNotificationPayload server_notification;
    } payload;

} Message;

void print_message(Message* message);

Message create_user_chat_msg(const char* text, const char* username);

Message create_user_login_msg(const char* username);

Message create_client_connected();

Message create_client_on_queue();

Message create_task_request_msg(enum TaskType type);

Message create_task_status_msg(enum TaskType type, enum TaskStatus status);

Message create_file_header_message(const char* name, uint32_t size);

Message create_file_content_message(const uint8_t* content, uint32_t size);

Message create_file_end_message();

Message create_seq_start_msg();

Message create_seq_end_msg();

Message create_heap_seq_msg(const uint8_t* payload, uint32_t payload_size);

Message create_status_msg(bool status, const char* format, ...);

Message create_connected_clients_msg(uint8_t count);

Message create_server_notification(const char* format, ...);

#endif