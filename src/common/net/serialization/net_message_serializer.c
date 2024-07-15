#include <assert.h>
#include "messages/message_types.h"
#include "net/serialization/net_message_serializer.h"
#include "net/serialization/net_serializer.h"

void ns_serialize_message(const Message* message, uint8_t* buffer, size_t* buffer_size)
{
    uint8_t* buffer_ptr = buffer;

    // Serializes message type
    ns_push_32(&buffer_ptr, &message->type);
    ns_push_32(&buffer_ptr, &message->net_payload_length);

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
        ns_push_64(&buffer_ptr, (const uint64_t*)&user_chat->time);
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
        ns_push_32(&buffer_ptr, &file_header->size);
        break;
    }
    case MSGT_FILE_CONTENT: {
        const FileContentPayload* file_content = &message->payload.file_content;
        ns_push_32(&buffer_ptr, (const uint32_t*)&file_content->content_size);
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

    case MSGT_HEAP_SEQUENCE: {
        const HeapSequencePayload* heap_seq = &message->payload.heap_sequence;
        ns_push_byte_array(&buffer_ptr, (const uint8_t*)heap_seq->payload, message->net_payload_length);
        break;
    }

    case MSGT_SERVER_NOTIFICATION: {
        const ServerNotificationPayload* server_notification = &message->payload.server_notification;
        ns_push_64(&buffer_ptr, (const uint64_t*)&server_notification->time);
        ns_push_byte_array(&buffer_ptr, (const uint8_t*)server_notification->text, sizeof(server_notification->text));
        break;
    }
    case MSGT_TASK_REQUEST:
    case MSGT_TASK_STATUS: {

        // Pushes specific data and get tagged task
        const TaggedTask* tagged_task;

        switch (message->type) {
        case MSGT_TASK_REQUEST:
            tagged_task = &message->payload.task_request.tagged_task;
            break;

        case MSGT_TASK_STATUS: {
            const TaskStatusPayload* task = &message->payload.task_status;
            ns_push_32(&buffer_ptr, (const uint32_t*)&task->task_status);
            tagged_task = &message->payload.task_status.tagged_task;
            break;
        }

        default:
            assert(false);
            break;
        }

        // Pushes tagged task data
        ns_push_32(&buffer_ptr, (const uint32_t*)&tagged_task->task_type);

        switch (tagged_task->task_type) {
        case TASK_USERS:
            ns_push_byte_array(&buffer_ptr, (const uint8_t*)&tagged_task->data.users.show_ip, sizeof(tagged_task->data.users.show_ip));
            ns_push_byte_array(&buffer_ptr, (const uint8_t*)&tagged_task->data.users.show_id, sizeof(tagged_task->data.users.show_id));
            break;

        case TASK_CLIENT_UPLOAD_FILE:
            ns_push_byte_array(&buffer_ptr, (const uint8_t*)tagged_task->data.file_upload.filename, sizeof(tagged_task->data.file_upload.filename));
            ns_push_byte_array(&buffer_ptr, (const uint8_t*)tagged_task->data.file_upload.user_filepath, sizeof(tagged_task->data.file_upload.user_filepath));
            break;

        case TASK_CLIENT_DOWNLOAD_FILE:
            ns_push_byte_array(&buffer_ptr, (const uint8_t*)tagged_task->data.file_download.filename, sizeof(tagged_task->data.file_download.filename));
            ns_push_byte_array(&buffer_ptr, (const uint8_t*)&tagged_task->data.file_download.by_id, sizeof(tagged_task->data.file_download.by_id));
            break;

        case TASK_FILES:
            break;

        case TASK_REMOVE_FILE:
            ns_push_byte_array(&buffer_ptr, (const uint8_t*)&tagged_task->data.remove.remove_all, sizeof(tagged_task->data.remove.remove_all));
            ns_push_32(&buffer_ptr, (const uint32_t*)&tagged_task->data.remove.file_id);
            break;

        default:
            printf("Unimplemented serialization for task message type %i\n", tagged_task->task_type);
            exit(EXIT_FAILURE);
            break;
        }

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

    // Pops type and length
    ns_pop_32(&buffer_ptr, &message->type);
    ns_pop_32(&buffer_ptr, &message->net_payload_length);

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
        ns_pop_64(&buffer_ptr, (uint64_t*)&user_chat->time);
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
        ns_pop_32(&buffer_ptr, &file_header->size);
        break;
    }
    case MSGT_FILE_CONTENT: {
        FileContentPayload* file_content = &message->payload.file_content;
        ns_pop_32(&buffer_ptr, (uint32_t*)&file_content->content_size);
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
        ns_pop_64(&buffer_ptr, (uint64_t*)&server_notification->time);
        ns_pop_byte_array(&buffer_ptr, (uint8_t*)server_notification->text, sizeof(server_notification->text));
        break;
    }

    case MSGT_TASK_REQUEST:
    case MSGT_TASK_STATUS: {

        // Pushes specific data and get tagged task
        TaggedTask* tagged_task;

        switch (message->type) {
        case MSGT_TASK_REQUEST:
            tagged_task = &message->payload.task_request.tagged_task;
            break;

        case MSGT_TASK_STATUS: {
            TaskStatusPayload* task = &message->payload.task_status;
            ns_pop_32(&buffer_ptr, (uint32_t*)&task->task_status);
            tagged_task = &message->payload.task_status.tagged_task;
            break;
        }

        default:
            assert(false);
            break;
        }

        // Pushes tagged task data
        ns_pop_32(&buffer_ptr, (uint32_t*)&tagged_task->task_type);

        switch (tagged_task->task_type) {
        case TASK_USERS:
            ns_pop_byte_array(&buffer_ptr, (uint8_t*)&tagged_task->data.users.show_ip, sizeof(tagged_task->data.users.show_ip));
            ns_pop_byte_array(&buffer_ptr, (uint8_t*)&tagged_task->data.users.show_id, sizeof(tagged_task->data.users.show_id));
            break;

        case TASK_CLIENT_UPLOAD_FILE:
            ns_pop_byte_array(&buffer_ptr, (uint8_t*)tagged_task->data.file_upload.filename, sizeof(tagged_task->data.file_upload.filename));
            ns_pop_byte_array(&buffer_ptr, (uint8_t*)tagged_task->data.file_upload.user_filepath, sizeof(tagged_task->data.file_upload.user_filepath));
            break;

        case TASK_CLIENT_DOWNLOAD_FILE:
            ns_pop_byte_array(&buffer_ptr, (uint8_t*)tagged_task->data.file_download.filename, sizeof(tagged_task->data.file_download.filename));
            ns_pop_byte_array(&buffer_ptr, (uint8_t*)&tagged_task->data.file_download.by_id, sizeof(tagged_task->data.file_download.by_id));
            break;

        case TASK_FILES:
            break;
        case TASK_REMOVE_FILE:
            ns_pop_byte_array(&buffer_ptr, (uint8_t*)&tagged_task->data.remove.remove_all, sizeof(tagged_task->data.remove.remove_all));
            ns_pop_32(&buffer_ptr, (uint32_t*)&tagged_task->data.remove.file_id);
            break;
        default:
            printf("Unimplemented serialization for task message type %i\n", tagged_task->task_type);
            exit(EXIT_FAILURE);
            break;
        }

        break;
    }

    default:
        printf("Unimplemented deserialization for message type %i\n", message->type);
        exit(EXIT_FAILURE);
        break;
    }
}