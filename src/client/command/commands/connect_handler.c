#include "states_fsm.h"
#include "ui/ui.h"

Error* connect_handler(uint8_t argc, char** argv)
{
    Client* data = get_client();

    // Localhost with default port
    if (argc == 1) {
        strcpy(data->connection_details.server_ip, "127.0.0.1");
        data->connection_details.status_port = DEFAULT_STATUS_PORT;
    }
    // Default port
    else if (argc == 2) {
        strcpy(data->connection_details.server_ip, argv[1]);
        data->connection_details.status_port = DEFAULT_STATUS_PORT;
    }
    // Specifies IP and port
    else {
        strcpy(data->connection_details.server_ip, argv[1]);
        data->connection_details.status_port = atoi(argv[2]);
    }
    ui_set_log_text("Transitioning to connect state");
    state_handler_set_next(APP_STATE_CONNECT);
    return CREATE_ERR_OK;
}