#include "state_handler_utils.h"

Error* handle_state_offline()
{
    Client* data = get_client();
    // Gets username
    printf("%s", "Server IP: ");
    fgets(data->connection_details.server_ip, sizeof(data->connection_details.server_ip), stdin);

    printf("%s", "Server port: ");
    char port[6];
    fgets(port, sizeof(port), stdin);

    // Default IP
    if (data->connection_details.server_ip[0] == '\n') {
        strcpy(data->connection_details.server_ip, "127.0.0.1");
    } else {
        // Removes '\n' character in both inputs
        data->connection_details.server_ip[strlen(data->connection_details.server_ip) - 1] = '\0';
    }

    // Default port
    if (port[0] == '\n') {
        strcpy(port, "8000");
    } else {
        // Removes '\n' character in both inputs
        port[strlen(port) - 1] = '\0';
    }

    data->connection_details.port = atoi(port);

    state_handler_set_next(APP_STATE_CONNECT);
    return CREATE_ERR_OK;
}