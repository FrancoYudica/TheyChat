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

            struct tm* time = localtime((time_t*)&user_chat->time);
            entry.time.hour = time->tm_hour;
            entry.time.minute = time->tm_min;

            // Sends chat entry to UI
            ui_add_chat_entry(&chat->ui, entry);

            // Sets up chat entry
            ChatEntry notification;
            notification.type = CHAT_ENTRY_SERVER_NOTIFICATION;
            sprintf(notification.data.server_notification.text, "Client %s sent a message!", user_chat->username);
            notification.time.hour = time->tm_hour;
            notification.time.minute = time->tm_min;

            ui_add_chat_entry(&chat->ui, notification);

            break;
        }
        case MSGT_CONNECTED_CLIENTS: {
            chat->ui.connected_count = message.payload.connected_clients.client_count;
            ui_refresh(&chat->ui);
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

ErrorCode process_command(Chat* chat, const char* command)
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

    return execute_command_processor(chat->client_data, command_type, arg);
}

extern void render_input_window(UI* ui, char* input);
extern void render_chat_window(UI*);

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
        render_input_window(&chat->ui, input);

        if (strlen(input) == 0)
            continue;

        char* trimmed_input = trim(input);
        bool is_command = trimmed_input[0] == '/' && strlen(trimmed_input) > 1;

        if (is_command) {
            chat->input_error = process_command(chat, (const char*)(trimmed_input + 1));
        } else {
            message = create_user_chat_msg(trimmed_input, data->username);
            chat->input_error = send_message((const Message*)&message, data->connection_context);

            uint32_t log_size = sizeof(chat->ui.log);
            char log[log_size];
            snprintf(log, log_size, "Sent `%s` to server.", trimmed_input);
            ui_set_log_text(&chat->ui, log);
        }

        if (IS_NET_ERROR(chat->input_error))
            break;
    }
    chat_exit(chat);
    return NULL;
}

ErrorCode handle_state_chat(ClientData* data, AppState* next_state)
{
    // Sets up chat data structure
    chat.client_data = data;
    chat.input_error = ERR_NET_OK;
    chat.messages_error = ERR_NET_OK;
    chat.active = true;

    // Initializes UI
    UI* ui = &chat.ui;

    ui_init(ui);
    ui->connected_count = 1;
    strcpy(ui->server_ip, data->connection_details.server_ip);

    // Renders the entire UI
    ui_refresh(ui);

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

    ui_free(&chat.ui);

    if (IS_NET_ERROR(chat.input_error))
        return chat.input_error;
    if (IS_NET_ERROR(chat.messages_error))
        return chat.messages_error;

    return ERR_NET_OK;
}