#include "state_handler_utils.h"

ErrorCode handle_state_login(ServerStateData* handler_data, AppState* next_state)
{
    Client* client = handler_data->client;
    while (true) {
        UserLoginMsg* user_login_msg;

        // Waits for login
        ErrorCode status = wait_for_message_type(&client->stream, client->sockfd, (Message**)&user_login_msg, MSGT_USER_LOGIN);

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
            ErrorCode status = send_message((Message*)status_msg, client->sockfd);
            if (IS_NET_ERROR(status))
                return status;
            free(status_msg);
        }
    }

    StatusMsg* status_msg = create_status_msg(STATUS_MSG_SUCCESS, "Login success");
    ErrorCode status = send_message((Message*)status_msg, client->sockfd);

    if (IS_NET_ERROR(status))
        return status;

    free(status_msg);

    printf("Client %d logged in with username %s\n", client->id, client->name);
    *next_state = APP_STATE_CHAT;
    return ERR_NET_OK;
}