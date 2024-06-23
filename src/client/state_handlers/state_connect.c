#include "state_handler_utils.h"

ErrorCode handle_state_connect(ClientData* data, AppState* next_state)
{

    // Establishes connection with serer
    printf(
        "Connecting to server ip (%s) and port (%d)...\n",
        data->connection_details.server_ip,
        data->connection_details.port);

    ErrorCode connection_status = net_client_create_socket(
        data->connection_details.port,
        data->connection_details.server_ip,
        &data->connection_context);

    // Unable to connect, goes back to offline state
    if (IS_NET_ERROR(connection_status)) {
        printf("Unable to connect to server... ErrorCode: %i\n", connection_status);
        *next_state = APP_STATE_OFFLINE;
        return ERR_NET_OK;
    }

    Message message;

    // Waits for connected, or on queue message
    ErrorCode status = wait_for_message(
        &data->stream,
        data->connection_context,
        &message);

    if (IS_NET_ERROR(status))
        return status;

    if (message.type == MSGT_CLIENT_ON_QUEUE)
        *next_state = APP_STATE_QUEUE;

    else if (message.type == MSGT_CLIENT_CONNECTED) {
        *next_state = APP_STATE_LOGIN;
    } else {
        printf("Received unexpected type: %i\n", message.type);
        *next_state = APP_STATE_OFFLINE;
    }

    return status;
}