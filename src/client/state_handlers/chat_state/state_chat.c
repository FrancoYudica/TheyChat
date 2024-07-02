#include <unistd.h>
#include <pthread.h>
#include "state_handler_utils.h"
#include "string/utils.h"
#include "command/command_processor.h"
#include "chat_state/chat.h"
#include "ui/ui.h"
#include "ui/input_handler.h"

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
            chat_entry_format_time(&entry, user_chat->time);

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
            chat_entry_format_time(&entry, server_notification->time);

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

static Error* input_callback(const char* input)
{
    static Message message;
    Chat* chat = (Chat*)input_handler_get_user_data();
    ClientData* data = chat->client_data;

    message = create_user_chat_msg(input, data->username);
    Error* error = send_message((const Message*)&message, data->connection_context);
    char log[256];
    snprintf(log, 256, "Sent `%s` to server.", input);
    ui_set_log_text(log);
    return error;
}

Error* handle_state_chat(ClientData* data)
{
    // Sets up chat data structure
    chat.client_data = data;
    chat.messages_error = CREATE_ERR_OK;
    chat.active = true;

    // Renders the entire UI
    ui_refresh();

    // Initializes mutexes and condition
    pthread_mutex_init(&chat.mutex, NULL);
    pthread_mutex_init(&chat.condition_mutex, NULL);
    pthread_cond_init(&chat.exit_condition, NULL);

    // Initializes messages thread for receiving server messages
    pthread_create(&chat.messages_thread, NULL, handle_messages, &chat);

    // Sets callback to NULL, so it can safely change the user data
    input_handler_set_input_callback(NULL);
    input_handler_set_user_data((void*)&chat);
    input_handler_set_input_callback(input_callback);

    // Waits for exit condition
    pthread_cond_wait(&chat.exit_condition, &chat.condition_mutex);

    state_handler_set_next(APP_STATE_DISCONNECT);

    ui_free();

    if (IS_NET_ERROR(chat.messages_error))
        return chat.messages_error;

    return CREATE_ERR_OK;
}