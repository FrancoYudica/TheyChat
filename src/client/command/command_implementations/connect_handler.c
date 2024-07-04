#include "states_fsm.h"

Error* connect_handler(uint8_t argc, char** argv)
{
    Client* data = get_client();

    // Localhost with default port
    if (argc == 1) {
        strcpy(data->connection_details.server_ip, "127.0.0.1");
        data->connection_details.port = 8000;
    }
    // Default port
    else if (argc == 2) {
        strcpy(data->connection_details.server_ip, argv[1]);
        data->connection_details.port = 8000;
    }
    // Specifies IP and port
    else {
        strcpy(data->connection_details.server_ip, argv[1]);
        data->connection_details.port = atoi(argv[2]);
    }

    state_handler_set_next(APP_STATE_CONNECT);
    return CREATE_ERR_OK;
}