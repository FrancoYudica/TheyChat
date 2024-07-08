#include "state_handler_utils.h"
#include "server_task/server_task_handler.h"
#include "states_fsm.h"

static pthread_t s_receive_thread;
static Error* s_receive_error = CREATE_ERR_OK;
static bool s_receiving = false;

/// @brief Receives messages from the server
void* handle_messages(void* arg)
{
    Client* client = get_client();

    Message message;
    s_receiving = true;

    while (s_receiving) {

        // Waits for server message
        s_receive_error = wait_for_message(&client->status_connection, &message);

        if (IS_NET_ERROR(s_receive_error)) {

            // Displays error it it's not a disconnection
            if (s_receive_error->code == ERR_NET_CONNECTION_CLOSED) {
                free_error(s_receive_error);
                s_receive_error = CREATE_ERR_OK;
            } else {
                ui_push_text_entry(
                    TEXT_ENTRY_TYPE_WARNING,
                    "%s",
                    s_receive_error->message);
                return NULL;
            }
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
            break;
        }
    }
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
    Client* client = get_client();

    message = create_user_chat_msg(input, client->username);
    Error* error = send_message((const Message*)&message, &client->status_connection);
    ui_set_log_text("Sent `%s` to server.", input);
    return error;
}

static void state_chat_exit()
{
    ui_push_text_entry(
        TEXT_ENTRY_TYPE_LOG,
        "%s",
        "Exiting chat...");

    server_task_handler_free();
}

Error* handle_state_chat()
{
    state_handler_set_exit_callback(state_chat_exit);

    // Initializes server command handler
    server_task_handler_init();

    Client* client = get_client();

    // Renders the entire UI
    ui_refresh();

    ui_set_input_prompt("Type message:");

    // Initializes messages thread for receiving server messages
    pthread_create(&s_receive_thread, NULL, handle_messages, NULL);
    set_thread_name(s_receive_thread, "Chat receive");

    // Sets callback to NULL, so it can safely change the user client
    input_handler_set_input_callback(NULL);
    input_handler_set_command_callback(NULL);
    input_handler_set_input_callback(input_callback);
    input_handler_set_command_callback(command_callback);

    // Waits for exit condition
    state_handler_wait_next_state_cond();
    pthread_detach(s_receive_thread);
    unregister_thread(s_receive_thread);

    s_receiving = false;

    if (IS_NET_ERROR(s_receive_error))
        return s_receive_error;

    return CREATE_ERR_OK;
}