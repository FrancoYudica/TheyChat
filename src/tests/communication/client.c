#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "message.h"
#include "message_types.h"
#include "net/serialization/net_message_serializer.h"
#include "net/net_communication.h"

#define SERVER_PORT 8000

int main()
{

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
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    // Establishes connection with server
    int status = connect(sockfd, (const struct sockaddr*)&server_addr, sizeof(const struct sockaddr_in));
    if (status == -1)
    {
        perror("Unable to connect");
        exit(EXIT_FAILURE);
    }

    NetworkStream network_stream;
    init_net_stream(&network_stream);

    // Receives stream endlessly
    receive(&network_stream, sockfd);
    printf("Received %d bytes\n", network_stream.written_bytes);
    // Pops message from stream
    Message *message;
    while(stream_pop_message_by_type(MESSAGE_TYPE_USER_CHAT, &message, &network_stream))
    {
        printf("Message popped\n");
        // Displays and frees memory
        print_message(message);
        free(message);
    }

    close(sockfd);
    return EXIT_SUCCESS;
}