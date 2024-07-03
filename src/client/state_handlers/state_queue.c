#include "state_handler_utils.h"

Error* handle_state_queue()
{
    Client* data = get_client();
    Message message;
    Error* err;
    printf("You are on queue, please wait...\n");

    // Waits for connection
    bool waiting_in_queue = true;
    while (waiting_in_queue) {
        err = wait_for_message(&data->stream, data->connection_context, &message);

        if (IS_NET_ERROR(err))
            return err;

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

    state_handler_set_next(APP_STATE_LOGIN);
    return err;
}