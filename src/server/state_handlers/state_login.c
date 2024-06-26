#include "state_handler_utils.h"
#include "broadcast_message.h"

Error* handle_state_login(ServerStateData* handler_data, AppState* next_state)
{
    Client* client = handler_data->client;
    bool logged = false;
    Message message;

    Error* err;

    while (!logged) {

        // Waits for login
        err = wait_for_message_type(
            &client->stream,
            client->connection_context,
            &message,
            MSGT_USER_LOGIN);

        if (IS_NET_ERROR(err))
            return err;

        const char* username = message.payload.user_login.username;
        logged = NULL == client_list_find_by_name(handler_data->server->client_list, username);

        if (logged) {
            strncpy(client->name, username, sizeof(client->name));
        } else {
            // Tell client that the username is invalid
            char text_buffer[128];
            sprintf(text_buffer, "A user named \"%s\" already exists", username);
            message = create_status_msg(STATUS_MSG_FAILURE, text_buffer);
            err = send_message(&message, client->connection_context);
            if (IS_NET_ERROR(err))
                return err;
        }
    }

    message = create_status_msg(STATUS_MSG_SUCCESS, "Login success");
    err = send_message(&message, client->connection_context);

    if (IS_NET_ERROR(err))
        return err;

    // Tells all the clients that a new client connected.
    {
        char text[128];
        sprintf(text, "Used named \"%s\" logged in!", client->name);
        message = create_server_notification(text);
        send_broadcast((const Message*)&message, handler_data->server);
    }

    server_client_count_update(handler_data->server);

    debug_print_client(client);
    printf(" logged in!\n");
    *next_state = APP_STATE_CHAT;
    return CREATE_ERR_OK;
}