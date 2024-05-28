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
#include "pthread.h"


uint32_t server_port = 8000;
char *server_ip = "127.0.0.1";

extern ErrorCode handle_state_connect(ClientData *data, AppState *next_state);
extern ErrorCode handle_state_login(ClientData *data, AppState *next_state);
extern ErrorCode handle_state_chat(ClientData *data, AppState *next_state);
extern ErrorCode handle_state_disconnect(ClientData *data, AppState *next_state);


void connect_to_server(ClientData *data)
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
    server_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    // Establishes connection with server
    int32_t connection_status = connect(data->sockfd, (const struct sockaddr*)&server_addr, sizeof(const struct sockaddr_in));
    if (connection_status == -1)
    {
        perror("Unable to connect");
        exit(EXIT_FAILURE);
    }
}




int main(int argc, char** argv)
{

    // Loads arguments
    int i = 0;
    while (++i < argc - 1)
    {
        char* parameter = argv[i++];

        if (!strcmp(parameter, "--port"))
        {
            char* port_str = argv[i];
            server_port = atoi(port_str);
        }

        else if (!strcmp(parameter, "--server_ip"))
        {
            server_ip = argv[i];
        }

        else
        {
            printf("Unrecognized parameters (%s, %s)", parameter, argv[i]);
        }
    }
    ClientData data;
    init_net_stream(&data.stream);
    AppState state = APP_STATE_CHAT;
    connect_to_server(&data);


    ASSERT_NET_ERROR(handle_state_connect(&data, &state));
    ASSERT_NET_ERROR(handle_state_login(&data, &state));
    ASSERT_NET_ERROR(handle_state_chat(&data, &state));
    handle_state_disconnect(&data, &state);
    return EXIT_SUCCESS;
}