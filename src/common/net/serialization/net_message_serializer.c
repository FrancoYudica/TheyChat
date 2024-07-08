#include "messages/message_types.h"
#include "net/serialization/net_message_serializer.h"
#include "net/serialization/net_serializer.h"

void ns_serialize_message(const Message* message, uint8_t* buffer, size_t* buffer_size)
{
    uint8_t* buffer_ptr = buffer;

    // Serializes message type
    ns_push_byte_array(&buffer_ptr, &message->type, sizeof(message->type));
    ns_push_long(&buffer_ptr, &message->net_payload_length);

    // Serializes properties for the corresponding message type
    switch (message->type) {
    // Empty messages
    case MSGT_FILE_END:
    case MSGT_SEQUENCE_START:
    case MSGT_SEQUENCE_END:
        break;

    case MSGT_USER_CHAT: {
        const UserChatPayload* user_chat = &message->payload.user_chat;
        ns_push_byte_array(&buffer_ptr, (const uint8_t*)user_chat->username, sizeof(user_chat->username));
        ns_push_byte_array(&buffer_ptr, (const uint8_t*)user_chat->text, sizeof(user_chat->text));
        ns_push_byte_array(&buffer_ptr, (const uint8_t*)user_chat->ip, sizeof(user_chat->ip));
        ns_push_long_long(&buffer_ptr, (const uint64_t*)&user_chat->time);
        break;
    }

    case MSGT_USER_LOGIN: {
        const UserLoginPayload* user_login = &message->payload.user_login;
        ns_push_byte_array(&buffer_ptr, (const uint8_t*)user_login->username, sizeof(user_login->username));
        break;
    }

    case MSGT_FILE_HEADER: {
        const FileHeaderPayload* file_header = &message->payload.file_header;
        ns_push_byte_array(&buffer_ptr, (const uint8_t*)file_header->name, sizeof(file_header->name));
        ns_push_long(&buffer_ptr, &file_header->size);
        break;
    }
    case MSGT_FILE_CONTENT: {
        const FileContentPayload* file_content = &message->payload.file_content;
        ns_push_long(&buffer_ptr, (const uint32_t*)&file_content->content_size);
        ns_push_byte_array(&buffer_ptr, (const uint8_t*)file_content->binary_payload, sizeof(file_content->binary_payload));
        break;
    }

    case MSGT_CLIENT_CONNECTED:
    case MSGT_CLIENT_ON_QUEUE: {
        const Bytes128Payload* bytes_128 = &message->payload.bytes_128;
        ns_push_byte_array(&buffer_ptr, (const uint8_t*)bytes_128->bytes, sizeof(bytes_128->bytes));
        break;
    }

    case MSGT_STATUS: {
        const StatusPayload* status_payload = &message->payload.status;
        ns_push_byte_array(&buffer_ptr, (const uint8_t*)&status_payload->status, sizeof(status_payload->status));
        ns_push_byte_array(&buffer_ptr, (const uint8_t*)status_payload->text, sizeof(status_payload->text));
        break;
    }

    case MSGT_CONNECTED_CLIENTS: {
        const ConnectedClientsPayload* connected_clients = &message->payload.connected_clients;
        ns_push_byte_array(&buffer_ptr, (const uint8_t*)&connected_clients->client_count, sizeof(connected_clients->client_count));
        break;
    }

    case MSGT_TASK_REQUEST: {
        const TaskRequestPayload* task = &message->payload.task_request;
        ns_push_byte_array(&buffer_ptr, (const uint8_t*)&task->task_type, sizeof(task->task_type));
        ns_push_byte_array(&buffer_ptr, (const uint8_t*)task->arg, sizeof(task->arg));
        break;
    }

    case MSGT_HEAP_SEQUENCE: {
        const HeapSequencePayload* heap_seq = &message->payload.heap_sequence;
        ns_push_byte_array(&buffer_ptr, (const uint8_t*)heap_seq->payload, message->net_payload_length);
        break;
    }

    case MSGT_SERVER_NOTIFICATION: {
        const ServerNotificationPayload* server_notification = &message->payload.server_notification;
        ns_push_long_long(&buffer_ptr, (const uint64_t*)&server_notification->time);
        ns_push_byte_array(&buffer_ptr, (const uint8_t*)server_notification->text, sizeof(server_notification->text));
        break;
    }
    default:
        printf("Unimplemented serialization for message type %i\n", message->type);
        exit(EXIT_FAILURE);
        break;
    }

    // Buffer size is calculated as the difference between the pointers
    *buffer_size = buffer_ptr - buffer;
}

void ns_deserialize_message(const uint8_t* buffer, Message* message)
{
    uint8_t* buffer_ptr = (uint8_t*)buffer;

    // Pops type
    ns_pop_byte_array(&buffer_ptr, &message->type, sizeof(message->type));

    // Pops payload length
    ns_pop_long(&buffer_ptr, &message->net_payload_length);

    // Serializes properties for the corresponding message type
    switch (message->type) {
    // Empty messages
    case MSGT_FILE_END:
    case MSGT_SEQUENCE_START:
    case MSGT_SEQUENCE_END:
        break;

    case MSGT_USER_CHAT: {
        UserChatPayload* user_chat = &message->payload.user_chat;
        ns_pop_byte_array(&buffer_ptr, (uint8_t*)user_chat->username, sizeof(user_chat->username));
        ns_pop_byte_array(&buffer_ptr, (uint8_t*)user_chat->text, sizeof(user_chat->text));
        ns_pop_byte_array(&buffer_ptr, (uint8_t*)user_chat->ip, sizeof(user_chat->ip));
        ns_pop_long_long(&buffer_ptr, (uint64_t*)&user_chat->time);
        break;
    }

    case MSGT_USER_LOGIN: {
        UserLoginPayload* user_login = &message->payload.user_login;
        ns_pop_byte_array(&buffer_ptr, (uint8_t*)user_login->username, sizeof(user_login->username));
        break;
    }

    case MSGT_FILE_HEADER: {
        FileHeaderPayload* file_header = &message->payload.file_header;
        ns_pop_byte_array(&buffer_ptr, (uint8_t*)file_header->name, sizeof(file_header->name));
        ns_pop_long(&buffer_ptr, &file_header->size);
        break;
    }
    case MSGT_FILE_CONTENT: {
        FileContentPayload* file_content = &message->payload.file_content;
        ns_pop_long(&buffer_ptr, (uint32_t*)&file_content->content_size);
        ns_pop_byte_array(&buffer_ptr, (uint8_t*)file_content->binary_payload, sizeof(file_content->binary_payload));
        break;
    }

    case MSGT_CLIENT_CONNECTED:
    case MSGT_CLIENT_ON_QUEUE: {
        Bytes128Payload* bytes_128 = &message->payload.bytes_128;
        ns_pop_byte_array(&buffer_ptr, (uint8_t*)bytes_128->bytes, sizeof(bytes_128->bytes));
        break;
    }

    case MSGT_STATUS: {
        StatusPayload* status_payload = &message->payload.status;
        ns_pop_byte_array(&buffer_ptr, (uint8_t*)&status_payload->status, sizeof(status_payload->status));
        ns_pop_byte_array(&buffer_ptr, (uint8_t*)status_payload->text, sizeof(status_payload->text));
        break;
    }
    case MSGT_CONNECTED_CLIENTS: {
        ConnectedClientsPayload* connected_clients = &message->payload.connected_clients;
        ns_pop_byte_array(&buffer_ptr, (uint8_t*)&connected_clients->client_count, sizeof(connected_clients->client_count));
        break;
    }
    case MSGT_TASK_REQUEST: {
        TaskRequestPayload* task = &message->payload.task_request;
        ns_pop_byte_array(&buffer_ptr, (uint8_t*)&task->task_type, sizeof(task->task_type));
        ns_pop_byte_array(&buffer_ptr, (uint8_t*)task->arg, sizeof(task->arg));
        break;
    }

    case MSGT_HEAP_SEQUENCE: {
        HeapSequencePayload* heap_seq = &message->payload.heap_sequence;

        // Allocates memory
        heap_seq->payload = malloc(message->net_payload_length);

        // Copies the buffer memory onto the newly allocated buffer
        ns_pop_byte_array(&buffer_ptr, (uint8_t*)heap_seq->payload, message->net_payload_length);
        break;
    }
    case MSGT_SERVER_NOTIFICATION: {
        ServerNotificationPayload* server_notification = &message->payload.server_notification;
        ns_pop_long_long(&buffer_ptr, (uint64_t*)&server_notification->time);
        ns_pop_byte_array(&buffer_ptr, (uint8_t*)server_notification->text, sizeof(server_notification->text));
        break;
    }
    default:
        printf("Unimplemented deserialization for message type %i\n", message->type);
        exit(EXIT_FAILURE);
        break;
    }
}