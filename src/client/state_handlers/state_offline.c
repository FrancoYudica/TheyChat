#include "state_handler_utils.h"

ErrorCode handle_state_offline(ClientData* data, AppState* next_state)
{
    // Gets username
    printf("%s", "Server IP: ");
    fgets(data->connection_details.server_ip, sizeof(data->connection_details.server_ip), stdin);

    printf("%s", "Server port: ");
    char port[6];
    fgets(port, sizeof(port), stdin);

    // Removes '\n' character in both inputs
    data->connection_details.server_ip[strlen(data->connection_details.server_ip) - 1] = '\0';
    port[strlen(port) - 1] = '\0';

    data->connection_details.port = atoi(port);

    // Initializes socket and connects to server
    *next_state = APP_STATE_CONNECT;
    return ERR_NET_OK;
}