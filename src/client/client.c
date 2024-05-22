#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "message.h"
#include "message_types.h"
#include "net/serialization/net_message_serializer.h"
#include "net/net_communication.h"

uint32_t server_port = 8000;
char *server_ip = "127.0.0.1";

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

    // Create TCP socket
    uint32_t sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
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
    int32_t connection_status = connect(sockfd, (const struct sockaddr*)&server_addr, sizeof(const struct sockaddr_in));
    if (connection_status == -1)
    {
        perror("Unable to connect");
        exit(EXIT_FAILURE);
    }

    NetworkStream network_stream;
    init_net_stream(&network_stream);

    UserChatMsg message;
    init_user_chat_msg(&message, "Received message!", "Unnamed client");

    Message *server_message;

    // Gets connection message
    net_status_t status = wait_for_message_type(&network_stream, sockfd, &server_message, MESSAGE_TYPE_USER_CHAT);
    print_message(server_message);
    free(server_message);

    while(1)
    {
        status = wait_for_message(&network_stream, sockfd, &server_message);

        if (IS_NET_ERROR(status))
        {
            if (status == NET_ERROR_PEER_DISCONNECTED)
                printf("Server disconnected\n");
            else   
                printf("Error code: %i\n", status);

            break;
        }
        print_message(server_message);
        free(server_message);
        
        send_message((const Message*)&message, sockfd);
        sleep(3);

    }

    printf("Closing socket fd %d\n", sockfd);

    if (close(sockfd) == -1)
        perror("close");

    return EXIT_SUCCESS;
}