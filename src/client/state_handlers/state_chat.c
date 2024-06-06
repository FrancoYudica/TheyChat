#include <unistd.h>
#include <pthread.h>
#include "state_handler_utils.h"
#include "string/utils.h"
#include "command/command_processor.h"

static ErrorCode input_error;
static ErrorCode messages_error;
static pthread_t input_thread, messages_thread;
static pthread_mutex_t chat_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t chat_exit_condition = PTHREAD_COND_INITIALIZER;
static bool chat_active = true;

void chat_exit()
{
    pthread_mutex_lock(&chat_mutex);
    chat_active = false;
    pthread_cond_signal(&chat_exit_condition);
    pthread_mutex_unlock(&chat_mutex);
}

void* handle_messages(void* arg)
{
    ClientData* data = (ClientData*)arg;
    while (true) {
        pthread_mutex_lock(&chat_mutex);
        if (!chat_active) {
            pthread_mutex_unlock(&chat_mutex);
            break;
        }
        pthread_mutex_unlock(&chat_mutex);

        Message* server_message;
        messages_error = wait_for_message(&data->stream, data->connection_context, &server_message);

        if (IS_NET_ERROR(messages_error))
            break;

        print_message(server_message);
        free(server_message);
    }
    chat_exit();
    return NULL;
}

ErrorCode process_command(ClientData* data, const char* command)
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

    execute_command_processor(data, command_type, arg);

    return err;
}

void* handle_input(void* arg)
{
    ClientData* data = (ClientData*)arg;
    char input_buffer[MAX_CHAT_TEXT_BYTES];

    while (true) {
        pthread_mutex_lock(&chat_mutex);
        if (!chat_active) {
            pthread_mutex_unlock(&chat_mutex);
            break;
        }
        pthread_mutex_unlock(&chat_mutex);

        fgets(input_buffer, MAX_CHAT_TEXT_BYTES, stdin);

        if (strlen(input_buffer) == 0)
            continue;

        // Replaces \n with null termination
        input_buffer[strlen(input_buffer) - 1] = '\0';

        char* trimmed_input = trim(input_buffer);
        bool is_command = trimmed_input[0] == '/' && strlen(trimmed_input) > 1;

        if (is_command) {
            input_error = process_command(data, (const char*)(trimmed_input + 1));
        } else {
            UserChatMsg* msg = create_user_chat_msg(trimmed_input, data->username);
            input_error = send_message((const Message*)msg, data->connection_context);
            free(msg);
        }

        if (IS_NET_ERROR(input_error))
            break;
    }
    chat_exit();
    return NULL;
}

ErrorCode handle_state_chat(ClientData* data, AppState* next_state)
{
    // Creates mutex and initializes condition
    pthread_mutex_t chat_mutex;
    pthread_mutex_init(&chat_mutex, NULL);
    pthread_cond_init(&chat_exit_condition, NULL);

    // Creates thread for handling input and receiving messages from server
    pthread_create(&input_thread, NULL, handle_input, data);
    pthread_create(&messages_thread, NULL, handle_messages, data);

    // Waits for exit condition
    pthread_cond_wait(&chat_exit_condition, &chat_mutex);

    *next_state = APP_STATE_DISCONNECT;

    if (IS_NET_ERROR(input_error))
        return input_error;
    if (IS_NET_ERROR(messages_error))
        return messages_error;

    return ERR_NET_OK;
}