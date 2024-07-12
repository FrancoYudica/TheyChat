#include "state_handler_utils.h"
#include "server_task/server_task_handler.h"
#include "states_fsm.h"

static pthread_t s_receive_thread;

/// @brief Receives messages from the server
void* handle_messages(void* arg)
{
    Error* err = CREATE_ERR_OK;
    Client* client = get_client();

    Message message;

    while (state_handler_get_current() == APP_STATE_CHAT) {

        // Waits for server message
        err = wait_for_message(&client->status_connection, &message);

        // Checks for errors
        if (IS_ERROR(err)) {

            switch (err->code) {

            // Connection closed manually by the client
            case ERR_NET_CONNECTION_CLOSED:
                free_error(err);
                break;

            // When server disconnects
            case ERR_NET_PEER_DISCONNECTED:
                free_error(err);
                ui_push_text_entry(
                    TEXT_ENTRY_TYPE_WARNING,
                    "Server disconnected");
                state_handler_set_next(APP_STATE_DISCONNECT);
                break;

            // When any other error happens
            default:
                ui_push_text_entry(
                    TEXT_ENTRY_TYPE_WARNING,
                    "%s",
                    err->message);

                free_error(err);
                state_handler_set_next(APP_STATE_DISCONNECT);
                break;
            }

            // After any error, exits the thread
            return NULL;
        }

        // Displays status
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
        8,
        CMD_HELP,
        CMD_DISCONNECT,
        CMD_QUIT,
        CMD_USERS,
        CMD_UPLOAD,
        CMD_DOWNLOAD,
        CMD_FILES,
        CMD_REMOVE);
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

    return CREATE_ERR_OK;
}