
#include "net/network.h"
#include <memory.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Structure to hold connection context information
struct ConnectionContext {
    /// @brief Socket file descriptor
    uint32_t socketfd;

    /// @brief Socket address structure
    struct sockaddr_in addr;
};

void net_init()
{
}

ConnectionContext* net_server_create_socket(
    const char* cert_file,
    const char* key_file,
    uint32_t port)
{
    ConnectionContext* context = (ConnectionContext*)malloc(sizeof(ConnectionContext));
    memset(context, 0, sizeof(ConnectionContext));

    // Create TCP socket
    int32_t sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    context->socketfd = sockfd;

    // IPv4
    context->addr.sin_family = AF_INET;

    // Sets the server ip to local ip
    context->addr.sin_addr.s_addr = INADDR_ANY;

    context->addr.sin_port = htons(port);

    int32_t bind_status = bind(context->socketfd,
        (const struct sockaddr*)&context->addr,
        sizeof(context->addr));

    // Bind socket to the server address
    if (bind_status < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    return context;
}

ConnectionContext* net_client_create_socket(
    uint32_t server_port,
    const char* server_ip)
{
    ConnectionContext* context = (ConnectionContext*)malloc(sizeof(ConnectionContext));
    memset(context, 0, sizeof(ConnectionContext));

    // Create TCP socket
    int32_t sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    context->socketfd = sockfd;

    // IPv4
    context->addr.sin_family = AF_INET;

    // Sets server port
    context->addr.sin_port = htons(server_port);

    // Sets the server ip to local ip
    inet_pton(AF_INET, server_ip, &context->addr.sin_addr);

    // Establishes connection with server
    int32_t connection_status = connect(
        context->socketfd,
        (const struct sockaddr*)&context->addr,
        sizeof(const struct sockaddr_in));

    if (connection_status == -1) {
        perror("Unable to connect");
        exit(EXIT_FAILURE);
    }
    return context;
}

void net_listen(ConnectionContext* context, uint32_t n)
{
    if (listen(context->socketfd, n) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
}
ErrorCode net_accept_connection(
    ConnectionContext* context,
    ConnectionContext** client_context_ref)
{
    ConnectionContext* client_context = malloc(sizeof(ConnectionContext));
    *client_context_ref = client_context;

    static socklen_t client_len = sizeof(struct sockaddr_in);

    // Accepts client connection
    int32_t client_sock_fd = accept(
        context->socketfd,
        (struct sockaddr*)&client_context->addr,
        &client_len);

    if (client_sock_fd == -1) {
        perror("accept failed");
        return ERR_NET_ACCEPT_FAIL;
    }

    client_context->socketfd = client_sock_fd;
    return ERR_NET_OK;
}

ErrorCode net_send(
    ConnectionContext* context,
    const uint8_t* buffer,
    uint32_t len,
    uint32_t* bytes_sent)
{
    int32_t sent = send(context->socketfd, buffer, len, 0);

    if (sent == -1) {
        perror("`send` in `send_buffer_to_socketfd failed`");
        return ERR_SEND_FAIL;
    }
    *bytes_sent = sent;
    return ERR_NET_OK;
}

ErrorCode net_receive(
    ConnectionContext* context,
    uint8_t* buffer,
    uint32_t size,
    uint32_t* bytes_read)
{
    int32_t read = recv(context->socketfd, buffer, size, 0);
    if (read == -1)
        return ERR_RECEIVE_FAIL;
    if (read == 0)
        return ERR_PEER_DISCONNECTED;

    *bytes_read = read;
    return ERR_NET_OK;
}

void net_close(ConnectionContext* context)
{
    close(context->socketfd);
}
void net_shutdown()
{
}
