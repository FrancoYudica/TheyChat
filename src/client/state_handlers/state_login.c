#include "state_handler_utils.h"

ErrorCode handle_state_login(ClientData* data, AppState* next_state)
{
    while (true) {
        // Gets username
        printf("%s", "Enter username: ");
        fgets(data->username, sizeof(data->username), stdin);

        // Replaces '\n'
        data->username[strlen(data->username) - 1] = '\0';

        // Sends message telling that the username
        UserLoginMsg* login_msg = create_user_login_msg(data->username);
        send_message((const Message*)login_msg, data->connection_context);
        free(login_msg);

        // Waits confirmation of the login
        StatusMsg* status_message;
        ErrorCode status = wait_for_message_type(&data->stream, data->connection_context, (Message**)&status_message, MSGT_STATUS);

        if (IS_NET_ERROR(status))
            return status;

        print_message((Message*)status_message);

        if (status_message->status == STATUS_MSG_FAILURE) {
            printf("%s\n", status_message->text);
        } else {
            break;
        }
    }
    *next_state = APP_STATE_CHAT;
    return ERR_NET_OK;
}