#include "state_handler_utils.h"

static pthread_t s_receive_thread;
static Error* s_receive_error = CREATE_ERR_OK;

/// @brief Receives messages from the server
void* handle_messages(void* arg)
{
    Client* data = get_client();

    Message message;

    bool receiving = true;

    while (receiving) {

        // Waits for server message
        s_receive_error = wait_for_message(&data->stream, data->connection_context, &message);

        if (IS_NET_ERROR(s_receive_error)) {
            receiving = false;
            break;
        }

        switch (message.type) {
        case MSGT_USER_CHAT: {

            UserChatPayload* user_chat = &message.payload.user_chat;

            // Sets up chat entry and formats with sent time
            ChatEntry* entry = chat_entry_create_user(user_chat->username, user_chat->text);
            chat_entry_format_time(entry, user_chat->time);

            // Sends chat entry to UI
            ui_add_chat_entry(entry);
            break;
        }
        case MSGT_SERVER_NOTIFICATION: {
            ServerNotificationPayload* server_notification = &message.payload.server_notification;
            ui_push_text_entry(TEXT_ENTRY_TYPE_SERVER, "%s", server_notification->text);
        }
        case MSGT_CONNECTED_CLIENTS: {
            ui_set_connected_count(message.payload.connected_clients.client_count);
            ui_refresh();
            break;
        }
        default:
            ui_set_log_text("Received unexpected message type %s... Disconnecting", msg_get_type_name(message.type));
            receiving = false;
            break;
        }
    }
    state_handler_set_next(APP_STATE_DISCONNECT);
    return NULL;
}

static Error* command_callback(const char* input)
{
    return dispatch_command(
        input,
        4,
        CMD_HELP,
        CMD_DISCONNECT,
        CMD_QUIT,
        CMD_USERS);
}

static Error* input_callback(const char* input)
{
    static Message message;
    Client* data = get_client();

    message = create_user_chat_msg(input, data->username);
    Error* error = send_message((const Message*)&message, data->connection_context);
    ui_set_log_text("Sent `%s` to server.", input);
    return error;
}

Error* handle_state_chat()
{
    Client* data = get_client();

    // Renders the entire UI
    ui_refresh();

    ui_set_input_prompt("Type message:");

    // Initializes messages thread for receiving server messages
    pthread_create(&s_receive_thread, NULL, handle_messages, NULL);

    // Sets callback to NULL, so it can safely change the user data
    input_handler_set_input_callback(NULL);
    input_handler_set_command_callback(NULL);
    input_handler_set_input_callback(input_callback);
    input_handler_set_command_callback(command_callback);

    // Waits for exit condition
    state_handler_wait_next_state_cond();
    pthread_detach(s_receive_thread);

    if (IS_NET_ERROR(s_receive_error))
        return s_receive_error;

    return CREATE_ERR_OK;
}