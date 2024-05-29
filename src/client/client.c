#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "messages/message.h"
#include "common/application_states.h"
#include "messages/message_types.h"
#include "net/serialization/net_message_serializer.h"
#include "net/net_communication.h"
#include "client/client_data.h"
#include "client/states_fsm.h"

typedef struct
{
    uint32_t port;
    char *server_ip;
} ConnectionDetails;


/// @brief Sets up socket connection with server
void connect_to_server(const ConnectionDetails *connection_details, ClientData *data);


int main(int argc, char** argv)
{
    // Sets default connection details
    ConnectionDetails connection_details;
    connection_details.port = 8000;
    connection_details.server_ip = "127.0.0.1";

    // Loads arguments
    int i = 0;
    while (++i < argc - 1)
    {
        char* parameter = argv[i++];

        if (!strcmp(parameter, "--port"))
        {
            char* port_str = argv[i];
            connection_details.port = atoi(port_str);
        }

        else if (!strcmp(parameter, "--server_ip"))
            connection_details.server_ip = argv[i];

        else
        {
            printf("Unrecognized parameters (%s, %s)", parameter, argv[i]);
        }
    }

    // Initializes client data
    ClientData data;
    init_net_stream(&data.stream);

    connect_to_server((const ConnectionDetails*)&connection_details, &data);

    // Starts execution of FSM
    client_states_handler_fsm(&data, APP_STATE_CONNECT);

    return EXIT_SUCCESS;
}


void connect_to_server(const ConnectionDetails *connection_details, ClientData *data)
{
    // Create TCP socket
    data->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (data->sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    socklen_t server_len = sizeof(struct sockaddr_in);

    // Initialize server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(connection_details->port);
    inet_pton(AF_INET, connection_details->server_ip, &server_addr.sin_addr);

    // Establishes connection with server
    int32_t connection_status = connect(data->sockfd, (const struct sockaddr*)&server_addr, sizeof(const struct sockaddr_in));
    if (connection_status == -1)
    {
        perror("Unable to connect");
        exit(EXIT_FAILURE);
    }
}