#include <stdlib.h>
#include <stddef.h>
#include "message.h"
#include "message_types.h"
#include "net/net_communication.h"
#include <unistd.h>

#define SERVER_PORT 8000

int main()
{

    struct sockaddr_in server_addr;
    socklen_t client_len = sizeof(struct sockaddr_in);

    // Create TCP socket
    uint32_t sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;   // Sets the server ip to local ip
    server_addr.sin_port = htons(SERVER_PORT);

    // Bind socket to the server address
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Sets socket in passive mode, allowing future connections
    // in the previously bound port
    if (listen(sockfd, 1) == -1)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("SERVER: Listening. Waiting for client connections...\n");

    struct sockaddr_in client_address;

    // Accepts connection
    int client_sock_fd = accept(
        sockfd, 
        (struct sockaddr*)&client_address, 
        &client_len
    );

    if (client_sock_fd == -1) {
        perror("recvfrom failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    Message *message = (Message*)create_user_chat_msg("Hola, como estas?", "Franco");
    send_message(message, client_sock_fd);
    send_message(message, client_sock_fd);
    send_message(message, client_sock_fd);
    send_message(message, client_sock_fd);
    send_message(message, client_sock_fd);
    printf("Message successfully sent\n");

    sleep(5);

    printf("Closing sockets fd. Client %d, Server %d\n", client_sock_fd, sockfd);

    if (close(client_sock_fd) == -1)
        perror("close");

    if (close(sockfd) == -1)
        perror("close");

    return EXIT_SUCCESS;
}