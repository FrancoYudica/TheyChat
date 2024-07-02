#include "state_handler_utils.h"

Error* handle_state_login(ClientData* data)
{
    Message message;
    Error* err;

    while (true) {
        // Gets username
        printf("%s", "Enter username: ");
        fgets(data->username, sizeof(data->username), stdin);

        // Replaces '\n'
        data->username[strlen(data->username) - 1] = '\0';

        // Sends message telling that the username
        message = create_user_login_msg(data->username);
        send_message((const Message*)&message, data->connection_context);

        // Waits confirmation of the login
        err = wait_for_message_type(&data->stream, data->connection_context, &message, MSGT_STATUS);

        if (IS_NET_ERROR(err))
            return err;

        if (message.payload.status.status == STATUS_MSG_FAILURE) {
            printf("%s\n", message.payload.status.text);
        } else {
            break;
        }
    }
    state_handler_set_next(APP_STATE_CHAT);
    return CREATE_ERR_OK;
}