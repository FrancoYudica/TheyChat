#include "messages/message.h"
#include "messages/message_types.h"
#include <time.h>

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

    case MSGT_COMMAND: {
        printf(
            "command_type: %i, arg: %s}\n",
            message->payload.command.command_type,
            message->payload.command.arg);
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

Message create_command_msg(uint8_t type, const char* arg)
{
    Message message;
    CommandPayload* command = &message.payload.command;

    message.type = MSGT_COMMAND;
    message.net_payload_length = sizeof(command->command_type) + sizeof(command->arg);
    command->command_type = type;

    if (arg != NULL)
        strcpy(command->arg, arg);
    else
        command->arg[0] = '\0';
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
Message create_status_msg(bool status, const char* text)
{
    Message message;
    StatusPayload* status_payload = &message.payload.status;

    message.type = MSGT_STATUS;
    message.net_payload_length = sizeof(status_payload->status) + sizeof(status_payload->text);
    status_payload->status = status;

    memset(status_payload->text, '\0', sizeof(status_payload->text));

    if (text != NULL)
        strcpy(status_payload->text, text);
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
