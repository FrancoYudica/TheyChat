#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "messages/message.h"
#include "application_states.h"
#include "messages/message_types.h"
#include "net/serialization/net_message_serializer.h"
#include "net/net_communication.h"
#include "client_data.h"
#include "states_fsm.h"

typedef struct
{
    uint32_t port;
    char* server_ip;
} ConnectionDetails;

/// @brief Sets up socket connection with server
void connect_to_server(const ConnectionDetails* connection_details, ClientData* data);

int main(int argc, char** argv)
{
    // Sets default connection details
    ConnectionDetails connection_details;
    connection_details.port = 8000;
    connection_details.server_ip = "127.0.0.1";

    // Loads arguments
    int i = 0;
    while (++i < argc - 1) {
        char* parameter = argv[i++];

        if (!strcmp(parameter, "--port")) {
            char* port_str = argv[i];
            connection_details.port = atoi(port_str);
        }

        else if (!strcmp(parameter, "--server_ip"))
            connection_details.server_ip = argv[i];

        else {
            printf("Unrecognized parameters (%s, %s)", parameter, argv[i]);
        }
    }

    // Initializes client data
    ClientData data;
    init_net_stream(&data.stream);

    // Initializes socket and connects to server
    net_client_init_socket(connection_details.port, connection_details.server_ip, &data.connection_context);
    net_connect_to_server(&data.connection_context);

    // Starts execution of FSM
    client_states_handler_fsm(&data, APP_STATE_CONNECT);

    return EXIT_SUCCESS;
}
