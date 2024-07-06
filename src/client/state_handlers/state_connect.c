#include "state_handler_utils.h"
#include "ui/ui.h"

Error* handle_state_connect()
{
    Client* client = get_client();

    ui_set_log_text(
        "Connecting to server ip (%s) and port (%d)...",
        client->connection_details.server_ip,
        client->connection_details.port);

    // Establishes connection with serer
    Error* err = net_client_create_socket(
        client->connection_details.port,
        client->connection_details.server_ip,
        &client->status_connection.context);

    // Unable to connect, goes back to offline state
    if (IS_NET_ERROR(err)) {
        ui_set_log_text("Unable to connect. Error: \"%s\"", err->message);
        state_handler_set_next(APP_STATE_OFFLINE);
        return CREATE_ERR_OK;
    }

    Message message;

    // Waits for connected, or on queue message
    Error* status = wait_for_message(
        &client->status_connection,
        &message);

    if (IS_NET_ERROR(status))
        return status;

    if (message.type == MSGT_CLIENT_ON_QUEUE) {
        ui_set_connected_count(1);
        ui_set_server_ip(client->connection_details.server_ip);
        ui_set_tls_enabled(client->connection_details.tls_enabled);
        ui_set_connected(true);
        state_handler_set_next(APP_STATE_ONQUEUE);
    }

    else if (message.type == MSGT_CLIENT_CONNECTED) {
        ui_set_connected_count(1);
        ui_set_server_ip(client->connection_details.server_ip);
        ui_set_tls_enabled(client->connection_details.tls_enabled);
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