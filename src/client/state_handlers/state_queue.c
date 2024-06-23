#include "state_handler_utils.h"

ErrorCode handle_state_queue(ClientData* data, AppState* next_state)
{
    Message message;

    printf("You are on queue, please wait...\n");

    // Waits for connection
    bool waiting_in_queue = true;
    while (waiting_in_queue) {
        ErrorCode status = wait_for_message(&data->stream, data->connection_context, &message);

        if (IS_NET_ERROR(status))
            return status;

        // Client connected successfully;
        if (message.type == MSGT_CLIENT_CONNECTED) {
            printf("%s\n", message.payload.bytes_128.bytes);
            waiting_in_queue = false;
        }

        // Client in queue
        else if (message.type == MSGT_CLIENT_ON_QUEUE) {
            printf("%s\n", message.payload.bytes_128.bytes);
        } else {
            printf("Received unexpected file type %i\n", message.type);
        }
    }

    *next_state = APP_STATE_LOGIN;
    return ERR_NET_OK;
}