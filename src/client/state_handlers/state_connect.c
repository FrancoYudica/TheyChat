#include "state_handler_utils.h"

Error* handle_state_connect()
{
    Client* data = get_client();

    ui_set_log_text(
        "Connecting to server ip (%s) and port (%d)...",
        data->connection_details.server_ip,
        data->connection_details.port);

    // Establishes connection with serer
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
        ui_set_connected_count(1);
        ui_set_server_ip(data->connection_details.server_ip);
        ui_set_tls_enabled(data->connection_details.tls_enabled);
        ui_set_connected(true);
        state_handler_set_next(APP_STATE_LOGIN);
    } else {

        ui_set_log_text(
            "Received unexpected type: %i",
            message.type);

        state_handler_set_next(APP_STATE_OFFLINE);
    }

    return status;
}