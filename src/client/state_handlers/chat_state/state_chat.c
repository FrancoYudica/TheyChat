#include <unistd.h>
#include <pthread.h>
#include "state_handler_utils.h"
#include "string/utils.h"
#include "command/command_processor.h"
#include "chat_state/chat.h"
#include "chat_state/ui.h"

static Chat chat;

/// @brief Receives messages from the server
void* handle_messages(void* arg)
{
    Chat* chat = (Chat*)arg;
    ClientData* data = chat->client_data;

    Message message;

    while (true) {

        if (!chat->active)
            break;

        // Waits for server message
        chat->messages_error = wait_for_message(&data->stream, data->connection_context, &message);

        if (IS_NET_ERROR(chat->messages_error))
            break;

        switch (message.type) {
        case MSGT_USER_CHAT: {

            UserChatPayload* user_chat = &message.payload.user_chat;
            // Sets up chat entry
            ChatEntry entry;
            entry.type = CHAT_ENTRY_USER_TEXT;
            strncpy(entry.data.user_text.name, user_chat->username, MAX_USERNAME_BYTES);
            strncpy(entry.data.user_text.text, user_chat->text, MAX_CHAT_TEXT_BYTES);
            strncpy(entry.data.user_text.ip, user_chat->ip, MAX_IP_BYTES);

            // Format the time_info to a string
            struct tm* time = localtime((time_t*)&user_chat->time);
            strftime(entry.time_str, sizeof(entry.time_str), "%Y/%m/%d %H:%M:%S", time);

            // Sends chat entry to UI
            ui_add_chat_entry(entry);
            break;
        }
        case MSGT_SERVER_NOTIFICATION: {
            ServerNotificationPayload* server_notification = &message.payload.server_notification;

            // Sets up chat entry
            ChatEntry entry;
            entry.type = CHAT_ENTRY_SERVER_NOTIFICATION;
            strcpy(entry.data.server_notification.text, server_notification->text);

            // Format the time_info to a string
            struct tm* time = localtime((time_t*)&server_notification->time);
            strftime(entry.time_str, sizeof(entry.time_str), "%Y/%m/%d %H:%M:%S", time);

            entry.time.hour = time->tm_hour;
            entry.time.minute = time->tm_min;

            ui_add_chat_entry(entry);
        }
        case MSGT_CONNECTED_CLIENTS: {
            ui_set_connected_count(message.payload.connected_clients.client_count);
            ui_refresh();
            break;
        }
        default:
            printf("Received unexpected message type\n");
            print_message(&message);
            chat_exit(chat);
            break;
        }
    }

    chat_exit(chat);

    return NULL;
}

Error* process_command(ClientData* client_data, const char* command)
{
    uint8_t command_type = CMDT_NULL;
    const char* arg = NULL;

    // Gets command type from string
    if (starts_with(command, "disconnect")) {
        command_type = CMDT_DISCONNECT;
    } else if (starts_with(command, "users")) {
        command_type = CMDT_USERS;
    } else {

        char log[256];
        sprintf(log, "Unrecognized command: %s\n", command);
        ui_set_log_text(log);
    }

    if (command_type == CMDT_NULL)
        return CREATE_ERR_OK;

    return execute_command_processor(client_data, command_type, arg);
}

void* handle_input(void* arg)
{
    Chat* chat = (Chat*)arg;
    ClientData* data = chat->client_data;
    Message message;

    // Holds client input
    char input[MAX_CHAT_TEXT_BYTES];

    while (true) {

        usleep(1000); // Sleep for 20ms to prevent high CPU usage

        if (!chat->active)
            break;

        // Renders input window, and blocks current thread until input is received
        input[0] = '\0';
        ui_try_pop_input(input);

        if (strlen(input) == 0)
            continue;

        char* trimmed_input = trim(input);
        bool is_command = trimmed_input[0] == '/' && strlen(trimmed_input) > 1;

        if (is_command) {
            chat->input_error = process_command(chat->client_data, (const char*)(trimmed_input + 1));
        } else {
            message = create_user_chat_msg(trimmed_input, data->username);
            chat->input_error = send_message((const Message*)&message, data->connection_context);

            char log[256];
            snprintf(log, 256, "Sent `%s` to server.", trimmed_input);
            ui_set_log_text(log);
        }

        if (IS_NET_ERROR(chat->input_error))
            break;
    }
    chat_exit(chat);
    return NULL;
}

Error* handle_state_chat(ClientData* data, AppState* next_state)
{
    // Sets up chat data structure
    chat.client_data = data;
    chat.input_error = CREATE_ERR_OK;
    chat.messages_error = CREATE_ERR_OK;
    chat.active = true;

    // Initializes UI
    ui_init();
    ui_set_connected_count(1);
    ui_set_server_ip(data->connection_details.server_ip);
    ui_set_connected(true);
    ui_set_tls_enabled(data->connection_details.tls_enabled);
    // Renders the entire UI
    ui_refresh();

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

    ui_free();

    if (IS_NET_ERROR(chat.input_error))
        return chat.input_error;
    if (IS_NET_ERROR(chat.messages_error))
        return chat.messages_error;

    return CREATE_ERR_OK;
}