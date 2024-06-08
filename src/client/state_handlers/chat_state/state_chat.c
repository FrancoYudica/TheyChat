#include <unistd.h>
#include <pthread.h>
#include "state_handler_utils.h"
#include "string/utils.h"
#include "command/command_processor.h"
#include "chat_state/chat.h"

void* handle_messages(void* arg)
{
    ChatData* chat = (ChatData*)arg;
    ClientData* data = chat->client_data;

    while (true) {
        pthread_mutex_lock(&chat->mutex);
        if (!chat->active) {
            pthread_mutex_unlock(&chat->mutex);
            break;
        }
        pthread_mutex_unlock(&chat->mutex);

        Message* server_message;
        chat->messages_error = wait_for_message(&data->stream, data->connection_context, &server_message);

        if (IS_NET_ERROR(chat->messages_error))
            break;

        print_message(server_message);
        free(server_message);
    }

    chat_exit(chat);

    return NULL;
}

ErrorCode process_command(ChatData* chat, const char* command)
{
    ErrorCode err = ERR_NET_OK;

    uint8_t command_type = CMDT_NULL;
    const char* arg = NULL;

    // Gets command type from string
    if (starts_with(command, "disconnect")) {
        command_type = CMDT_DISCONNECT;
    } else if (starts_with(command, "users")) {
        command_type = CMDT_USERS;
    } else {
        printf("Unrecognized command: %s\n", command);
    }

    if (command_type == CMDT_NULL)
        return ERR_NET_OK;

    execute_command_processor(chat->client_data, command_type, arg);

    return err;
}

void* handle_input(void* arg)
{
    ChatData* chat = (ChatData*)arg;
    ClientData* data = chat->client_data;

    char input_buffer[MAX_CHAT_TEXT_BYTES];

    while (true) {
        pthread_mutex_lock(&chat->mutex);
        if (!chat->active) {
            pthread_mutex_unlock(&chat->mutex);
            break;
        }
        pthread_mutex_unlock(&chat->mutex);

        fgets(input_buffer, MAX_CHAT_TEXT_BYTES, stdin);

        if (strlen(input_buffer) == 0)
            continue;

        // Replaces \n with null termination
        input_buffer[strlen(input_buffer) - 1] = '\0';

        char* trimmed_input = trim(input_buffer);
        bool is_command = trimmed_input[0] == '/' && strlen(trimmed_input) > 1;

        if (is_command) {
            chat->input_error = process_command(chat, (const char*)(trimmed_input + 1));
        } else {
            UserChatMsg* msg = create_user_chat_msg(trimmed_input, data->username);
            chat->input_error = send_message((const Message*)msg, data->connection_context);
            free(msg);
        }

        if (IS_NET_ERROR(chat->input_error))
            break;
    }
    chat_exit(chat);
    return NULL;
}

ErrorCode handle_state_chat(ClientData* data, AppState* next_state)
{
    ChatData chat;
    chat.client_data = data;
    chat.input_error = ERR_NET_OK;
    chat.messages_error = ERR_NET_OK;
    chat.active = true;

    // Initializes mutexes and condition
    pthread_mutex_init(&chat.mutex, NULL);
    pthread_mutex_init(&chat.condition_mutex, NULL);
    pthread_cond_init(&chat.exit_condition, NULL);

    // Creates thread for handling input and receiving messages from server
    pthread_create(&chat.input_thread, NULL, handle_input, &chat);
    pthread_create(&chat.messages_thread, NULL, handle_messages, &chat);

    // Waits for exit condition
    pthread_cond_wait(&chat.exit_condition, &chat.condition_mutex);

    *next_state = APP_STATE_DISCONNECT;

    if (IS_NET_ERROR(chat.input_error))
        return chat.input_error;
    if (IS_NET_ERROR(chat.messages_error))
        return chat.messages_error;

    return ERR_NET_OK;
}