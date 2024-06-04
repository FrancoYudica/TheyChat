#include "state_handler_utils.h"
#include "broadcast_message.h"

ErrorCode handle_state_login(ServerStateData* handler_data, AppState* next_state)
{
    Client* client = handler_data->client;
    while (true) {
        UserLoginMsg* user_login_msg;

        // Waits for login
        ErrorCode status = wait_for_message_type(&client->stream, &client->connection_context, (Message**)&user_login_msg, MSGT_USER_LOGIN);

        if (IS_NET_ERROR(status))
            return status;

        strncpy(client->name, user_login_msg->user_base.username, sizeof(client->name));
        free(user_login_msg);

        // @todo check if name is valid
        bool is_valid = true;

        if (is_valid)
            break;

        // Tell client that the username is invalid
        else {
            char text_buffer[128];
            sprintf(text_buffer, "A user named \"%s\" already exists", client->name);
            StatusMsg* status_msg = create_status_msg(STATUS_MSG_FAILURE, text_buffer);
            ErrorCode status = send_message((Message*)status_msg, &client->connection_context);
            if (IS_NET_ERROR(status))
                return status;
            free(status_msg);
        }
    }

    StatusMsg* status_msg = create_status_msg(STATUS_MSG_SUCCESS, "Login success");
    ErrorCode status = send_message((Message*)status_msg, &client->connection_context);

    if (IS_NET_ERROR(status))
        return status;

    free(status_msg);

    // Tells all the clients that a new client connected
    {
        char text[128];
        sprintf(text, "Used named \"%s\" logged in!", client->name);
        UserChatMsg* msg = create_user_chat_msg(text, "SERVER");
        send_broadcast((const Message*)msg, handler_data);
        free(msg);
    }

    printf("Client %d logged in with username %s\n", client->id, client->name);
    *next_state = APP_STATE_CHAT;
    return ERR_NET_OK;
}