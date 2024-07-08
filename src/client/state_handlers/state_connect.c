#include "state_handler_utils.h"
#include "ui/ui.h"

Error* handle_state_connect()
{
    Client* client = get_client();
    Error* err;

    // Establishes status connection
    {
        ui_set_log_text(
            "Establishing status connection on ip (%s) and port (%d)...",
            client->connection_details.server_ip,
            client->connection_details.status_port);

        // Establishes connection with serer
        err = net_client_create_socket(
            client->connection_details.status_port,
            client->connection_details.server_ip,
            &client->status_connection.context);

        // Unable to connect, goes back to offline state
        if (IS_NET_ERROR(err)) {
            ui_push_text_entry(
                TEXT_ENTRY_TYPE_WARNING,
                "Unable to establish status connection. Error: \"%s\"",
                err->message);
            state_handler_set_next(APP_STATE_OFFLINE);
            return CREATE_ERR_OK;
        }
    }
    // Establishes command connection
    {
        ui_set_log_text(
            "Establishing command connection on ip (%s) and port (%d)...",
            client->connection_details.server_ip,
            client->connection_details.cmd_port);

        // Establishes connection with serer
        err = net_client_create_socket(
            client->connection_details.cmd_port,
            client->connection_details.server_ip,
            &client->cmd_connection.context);

        // Unable to connect, goes back to offline state
        if (IS_NET_ERROR(err)) {
            ui_push_text_entry(
                TEXT_ENTRY_TYPE_WARNING,
                "Unable to establish command connection. Error: \"%s\"",
                err->message);
            state_handler_set_next(APP_STATE_OFFLINE);
            return CREATE_ERR_OK;
        }
    }

    ui_set_log_text("Connections established successfully");

    Message message;

    // Waits for connected, or on queue message
    err = wait_for_message(
        &client->status_connection,
        &message);

    if (IS_NET_ERROR(err))
        return err;

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

    return err;
}