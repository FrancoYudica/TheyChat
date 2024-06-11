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

int main(int argc, char** argv)
{

    ClientData client;

    // Sets default connection details
    client.connection_details.port = 8000;
    client.connection_details.server_ip = "127.0.0.1";

    // Loads arguments
    int i = 0;
    while (++i < argc - 1) {
        char* parameter = argv[i++];

        if (!strcmp(parameter, "--port")) {
            char* port_str = argv[i];
            client.connection_details.port = atoi(port_str);
        }

        else if (!strcmp(parameter, "--server_ip"))
            client.connection_details.server_ip = argv[i];

        else {
            printf("Unrecognized parameters (%s, %s)", parameter, argv[i]);
        }
    }

    net_init();

    // Initializes network stream
    init_net_stream(&client.stream);

    // Initializes socket and connects to server
    printf(
        "Connecting to server ip (%s) and port (%d)...\n",
        client.connection_details.server_ip,
        client.connection_details.port);

    client.connection_context = net_client_create_socket(
        client.connection_details.port,
        client.connection_details.server_ip);

    // Starts execution of FSM
    printf("Entering application...\n");
    client_states_handler_fsm(&client, APP_STATE_CONNECT);

    net_shutdown();
    return EXIT_SUCCESS;
}
