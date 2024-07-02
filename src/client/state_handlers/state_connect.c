#include "state_handler_utils.h"

Error* handle_state_connect(ClientData* data)
{

    // Establishes connection with serer
    printf(
        "Connecting to server ip (%s) and port (%d)...\n",
        data->connection_details.server_ip,
        data->connection_details.port);

    Error* err = net_client_create_socket(
        data->connection_details.port,
        data->connection_details.server_ip,
        &data->connection_context);

    // Unable to connect, goes back to offline state
    if (IS_NET_ERROR(err)) {
        print_error(err);
        state_handler_set_next(APP_STATE_OFFLINE);
        return CREATE_ERR_OK;
    }

    Message message;

    // Waits for connected, or on queue message
    Error* status = wait_for_message(
        &data->stream,
        data->connection_context,
        &message);

    if (IS_NET_ERROR(status))
        return status;

    if (message.type == MSGT_CLIENT_ON_QUEUE) {
        state_handler_set_next(APP_STATE_ONQUEUE);
    }

    else if (message.type == MSGT_CLIENT_CONNECTED) {
        state_handler_set_next(APP_STATE_LOGIN);
    } else {
        printf("Received unexpected type: %i\n", message.type);
        state_handler_set_next(APP_STATE_OFFLINE);
    }

    return status;
}