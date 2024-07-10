#include <time.h>
#include <assert.h>
#include "messages/message.h"
#include "messages/message_types.h"

void print_message(Message* message)
{

    // Prints type and length
    printf(
        "MESSAGE{type: %s, net_payload_length: %d, ",
        msg_get_type_name(message->type),
        message->net_payload_length);

    // Prints specific data to each message
    switch (message->type) {
    case MSGT_USER_CHAT: {
        printf(
            "username: %s, text: %s}\n",
            message->payload.user_chat.username,
            message->payload.user_chat.text);
        break;
    }

    case MSGT_FILE_HEADER: {
        printf(
            "name: %s, size: %d}\n",
            message->payload.file_header.name,
            message->payload.file_header.size);
        break;
    }

    case MSGT_USER_LOGIN: {
        printf(
            "type: LOGIN, username: %s}\n",
            message->payload.user_login.username);
        break;
    }

    case MSGT_USER_LOGOUT: {
        printf("type: LOGOUT, ");
        break;
    }

    case MSGT_CLIENT_CONNECTED:
    case MSGT_CLIENT_ON_QUEUE: {
        printf("}\n");
        break;
    }

    case MSGT_STATUS: {
        printf(
            "status: %i, text: %s}\n",
            message->payload.status.status,
            message->payload.status.text);
        break;
    }

    case MSGT_CONNECTED_CLIENTS: {
        printf(
            "client count: %i}\n",
            message->payload.connected_clients.client_count);
        break;
    }

    case MSGT_TASK_REQUEST: {
        printf(
            "task_type: %i}\n",
            message->payload.task_request.tagged_task.task_type);
        break;
    }
    case MSGT_TASK_STATUS: {
        printf(
            "task_type: %i, task_status: %i}\n",
            message->payload.task_status.tagged_task.task_type,
            message->payload.task_status.task_status);
        break;
    }

    case MSGT_SEQUENCE_START:
    case MSGT_SEQUENCE_END:
    case MSGT_HEAP_SEQUENCE:
        printf("}\n");
        break;

    default:
        break;
    }
}

Message create_user_chat_msg(const char* text, const char* username)
{
    Message message;
    UserChatPayload* chat_payload = &message.payload.user_chat;
    message.type = MSGT_USER_CHAT;
    message.net_payload_length = sizeof(chat_payload->username) + sizeof(chat_payload->text) + sizeof(chat_payload->ip) + sizeof(chat_payload->time);
    strcpy(chat_payload->text, text);
    strcpy(chat_payload->username, username);
    chat_payload->time = time(NULL);
    return message;
}

Message create_user_login_msg(const char* username)
{
    Message message;
    UserLoginPayload* login_payload = &message.payload.user_login;

    message.type = MSGT_USER_LOGIN;
    message.net_payload_length = sizeof(login_payload->username);
    strcpy(login_payload->username, username);
    return message;
}

Message create_client_connected()
{

    Message message;
    Bytes128Payload* bytes_128 = &message.payload.bytes_128;
    static const char text[] = "Welcome to TheyChat!";

    message.type = MSGT_CLIENT_CONNECTED;
    message.net_payload_length = 128;

    strcpy(bytes_128->bytes, text);
    return message;
}

Message create_client_on_queue()
{
    Message message;
    Bytes128Payload* bytes_128 = &message.payload.bytes_128;
    static const char text[] = "Connected to server. In client queue...";

    message.type = MSGT_CLIENT_ON_QUEUE;
    message.net_payload_length = 128;

    strcpy(bytes_128->bytes, text);
    return message;
}

/// @brief Initializes memory and adds to net_payload_length, considering the TaskType
static void initialize_tagged_task(
    Message* message,
    TaggedTask* tagged_task,
    enum TaskType type)
{

    message->net_payload_length += sizeof(tagged_task->task_type);
    tagged_task->task_type = type;
    switch (type) {
    case TASK_USERS:
        message->net_payload_length += sizeof(TaskUsersDada);
        break;

    case TASK_CLIENT_UPLOAD_FILE:
        message->net_payload_length += sizeof(TaskFileUploadData);
        break;
    case TASK_CLIENT_DOWNLOAD_FILE:
        message->net_payload_length += sizeof(TaskFileDownloadData);
        break;
    default:
        assert(false);
        break;
    }
    memset(&tagged_task->data, 0, sizeof(tagged_task->data));
}

Message create_task_request_msg(enum TaskType type)
{
    Message message;
    message.type = MSGT_TASK_REQUEST;
    message.net_payload_length = 0;
    initialize_tagged_task(
        &message,
        &message.payload.task_request.tagged_task,
        type);

    return message;
}

Message create_task_status_msg(enum TaskType type, enum TaskStatus status)
{
    Message message;
    TaskStatusPayload* task = &message.payload.task_status;

    message.type = MSGT_TASK_STATUS;
    message.net_payload_length = sizeof(task->task_status);
    task->task_status = status;
    initialize_tagged_task(
        &message,
        &message.payload.task_status.tagged_task,
        type);
    return message;
}

Message create_file_header_message(const char* name, uint32_t size)
{

    Message message;
    FileHeaderPayload* file_header = &message.payload.file_header;

    // Initializes message data
    message.type = MSGT_FILE_HEADER;
    message.net_payload_length = sizeof(file_header->name) + sizeof(file_header->size);

    strcpy(file_header->name, name);
    file_header->size = size;
    return message;
}

Message create_file_content_message(const uint8_t* content, uint32_t size)
{
    Message message;
    FileContentPayload* file_content = &message.payload.file_content;

    message.type = MSGT_FILE_CONTENT;
    message.net_payload_length = sizeof(file_content->content_size) + sizeof(file_content->binary_payload);
    file_content->content_size = size;
    memcpy(file_content->binary_payload, content, size);
    return message;
}

Message create_file_end_message()
{
    Message message;
    message.type = MSGT_FILE_END;
    message.net_payload_length = 0;
    return message;
}

Message create_seq_start_msg()
{
    Message message;
    message.type = MSGT_SEQUENCE_START;
    message.net_payload_length = 0;
    return message;
}

Message create_seq_end_msg()
{
    Message message;
    message.type = MSGT_SEQUENCE_END;
    message.net_payload_length = 0;
    return message;
}

Message create_heap_seq_msg(const uint8_t* payload, uint32_t payload_size)
{
    Message message;
    HeapSequencePayload* heap_sequence = &message.payload.heap_sequence;

    message.type = MSGT_HEAP_SEQUENCE;
    message.net_payload_length = payload_size;
    heap_sequence->payload = malloc(payload_size);
    memcpy(heap_sequence->payload, payload, payload_size);
    return message;
}
Message create_status_msg(bool status, const char* format, ...)
{
    Message message;
    StatusPayload* status_payload = &message.payload.status;

    message.type = MSGT_STATUS;
    message.net_payload_length = sizeof(status_payload->status) + sizeof(status_payload->text);
    status_payload->status = status;

    memset(status_payload->text, '\0', sizeof(status_payload->text));

    if (format != NULL) {
        char buffer[1024];
        va_list args;

        // Start the varargs processing
        va_start(args, format);

        // Format the string into the buffer
        vsnprintf(buffer, sizeof(buffer), format, args);

        // End the varargs processing
        va_end(args);

        strncpy(status_payload->text, buffer, sizeof(status_payload->text));
    }
    return message;
}

Message create_connected_clients_msg(uint8_t count)
{
    Message message;
    ConnectedClientsPayload* connected = &message.payload.connected_clients;
    message.type = MSGT_CONNECTED_CLIENTS;
    message.net_payload_length = sizeof(connected->client_count);
    connected->client_count = count;
    return message;
}

Message create_server_notification(const char* format, ...)
{
    Message message;
    ServerNotificationPayload* server_notification = &message.payload.server_notification;

    message.type = MSGT_SERVER_NOTIFICATION;
    message.net_payload_length = sizeof(server_notification->text) + sizeof(server_notification->time);
    server_notification->time = time(NULL);

    if (format != NULL) {
        char buffer[1024];
        va_list args;

        // Start the varargs processing
        va_start(args, format);

        // Format the string into the buffer
        vsnprintf(buffer, sizeof(buffer), format, args);

        // End the varargs processing
        va_end(args);

        strncpy(server_notification->text, buffer, sizeof(server_notification->text));
    }

    return message;
}
