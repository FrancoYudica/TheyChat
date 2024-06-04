#include "state_handler_utils.h"

ErrorCode handle_state_connect(ClientData* data, AppState* next_state)
{

    // Waits for connection
    bool waiting_in_queue = true;
    while (waiting_in_queue) {
        Message* server_message;
        ErrorCode status = wait_for_message(&data->stream, &data->connection_context, &server_message);
        assert(!IS_NET_ERROR(status));

        // Client connected successfully;
        if (server_message->header.type == MSGT_CLIENT_CONNECTED) {
            printf("%s\n", ((Bytes128Msg*)server_message)->bytes);
            waiting_in_queue = false;
        }

        // Client in queue
        else if (server_message->header.type == MSGT_CLIENT_ON_QUEUE) {
            printf("%s\n", ((Bytes128Msg*)server_message)->bytes);
        } else {
            printf("Received unexpected file type %i\n", server_message->header.type);
        }

        free(server_message);
    }
    // Sends confirmation
    StatusMsg* status_message = create_status_msg(true, "");
    ErrorCode status = send_message((const Message*)status_message, &data->connection_context);
    free(status_message);
    *next_state = APP_STATE_LOGIN;
    return status;
}