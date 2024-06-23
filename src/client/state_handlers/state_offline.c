#include "state_handler_utils.h"

ErrorCode handle_state_offline(ClientData* data, AppState* next_state)
{
    Message message;
    bool offline = true;
    while (offline) {
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
        printf(
            "Connecting to server ip (%s) and port (%d)...\n",
            data->connection_details.server_ip,
            data->connection_details.port);

        ErrorCode connection_status = net_client_create_socket(
            data->connection_details.port,
            data->connection_details.server_ip,
            &data->connection_context);

        if (IS_NET_ERROR(connection_status)) {
            printf("Unable to connect to server... ErrorCode: %i\n", connection_status);
        } else
            offline = false;
    }
    *next_state = APP_STATE_CONNECT;
    return ERR_NET_OK;
}