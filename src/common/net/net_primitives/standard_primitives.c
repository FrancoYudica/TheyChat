
#include "net/net_primitives.h"
#include "net/socket.h"
#include <memory.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdatomic.h>

// Structure to hold connection context information
struct ConnectionContext {
    /// @brief Socket file descriptor
    uint32_t socketfd;

    /// @brief Socket address structure
    struct sockaddr_in addr;

    atomic_bool closing;
};

void net_init()
{
}

static void init_connection_context(ConnectionContext* context)
{
    memset(context, 0, sizeof(ConnectionContext));
    context->socketfd = 0;
    atomic_store(&context->closing, false);
}

Error* net_server_create_socket(
    const char* cert_file,
    const char* key_file,
    uint32_t port,
    ConnectionContext** context_ref)
{
    *context_ref = (ConnectionContext*)malloc(sizeof(ConnectionContext));
    ConnectionContext* context = *context_ref;
    init_connection_context(context);

    // Create TCP socket
    int32_t sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        return CREATE_ERRNO(ERR_NET_SOCKET_CREATION_FAILED, "Unable to create socket");

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
    if (bind_status < 0)
        return CREATE_ERRNO(ERR_NET_FAILURE, "Unable to bind socket");

    return CREATE_ERR_OK;
}

Error* net_client_create_socket(
    uint32_t server_port,
    const char* server_ip,
    ConnectionContext** context_ref)
{
    *context_ref = (ConnectionContext*)malloc(sizeof(ConnectionContext));
    ConnectionContext* context = *context_ref;
    init_connection_context(context);

    // Create TCP socket
    int32_t sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
        return CREATE_ERRNO(ERR_NET_SOCKET_CREATION_FAILED, "Unable to connect create socket");

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

    if (connection_status == -1)
        return CREATE_ERRNO(ERR_NET_UNABLE_TO_CONNECT, "Unable to connect to server");

    return CREATE_ERR_OK;
}

Error* net_listen(
    ConnectionContext* context,
    uint32_t n)
{
    if (listen(context->socketfd, n) == -1)
        return CREATE_ERRNO(ERR_NET_FAILURE, "Listen failed");

    return CREATE_ERR_OK;
}

Error* net_accept_connection(
    ConnectionContext* context,
    ConnectionContext** client_context_ref)
{
    ConnectionContext* client_context = malloc(sizeof(ConnectionContext));
    *client_context_ref = client_context;
    init_connection_context(client_context);

    static uint32_t client_len = sizeof(struct sockaddr_in);

    // Accepts client connection
    int32_t client_sock_fd = accept(
        context->socketfd,
        (struct sockaddr*)&client_context->addr,
        &client_len);

    if (client_sock_fd == -1)
        return CREATE_ERRNO(ERR_NET_FAILURE, "Accept fail");

    client_context->socketfd = client_sock_fd;
    return CREATE_ERR_OK;
}

Error* net_send(
    ConnectionContext* context,
    const uint8_t* buffer,
    uint32_t len,
    uint32_t* bytes_sent)
{

    if (atomic_load(&context->closing))
        return CREATE_ERR(ERR_NET_CONNECTION_CLOSED, "Trying to send when connection is closed");

    int32_t sent = send(context->socketfd, buffer, len, 0);

    if (sent == -1)
        return CREATE_ERRNO(ERR_NET_FAILURE, "Send failure");

    if (bytes_sent != NULL)
        *bytes_sent = sent;

    return CREATE_ERR_OK;
}

Error* net_receive(
    ConnectionContext* context,
    uint8_t* buffer,
    uint32_t size,
    uint32_t* bytes_read)
{
    if (context == NULL || buffer == NULL)
        return CREATE_ERR(ERR_NET_FAILURE, "Invalid context or buffer");

    fd_set read_fds;
    struct timeval timeout;

    while (true) {
        FD_ZERO(&read_fds);
        FD_SET(context->socketfd, &read_fds);

        // Set timeout to 3 seconds
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;

        int32_t select_result = select(context->socketfd + 1, &read_fds, NULL, NULL, &timeout);
        if (select_result == -1)
            return CREATE_ERRNO(ERR_NET_FAILURE, "net_receive() `select()` failure");

        // TIMEOUT
        else if (select_result == 0)
            continue;

        // Checks if connection context is closing
        if (atomic_load(&context->closing)) {
            return CREATE_ERR(ERR_NET_CONNECTION_CLOSED, "Connection closed while trying to receive");
        }

        if (FD_ISSET(context->socketfd, &read_fds)) {

            int32_t read = recv(context->socketfd, buffer, size, 0);
            if (read == -1)
                return CREATE_ERRNO(ERR_NET_FAILURE, "net_receive() `recv()` failure");

            if (read == 0) {
                return CREATE_ERRNO(ERR_NET_PEER_DISCONNECTED, "Peer disconnected");
            }

            if (bytes_read != NULL) {
                *bytes_read = read;
            }

            return CREATE_ERR_OK;
        }
    }

    return CREATE_ERRNO(ERR_NET_FAILURE, "Unexpected error in select()");
}

Error* net_close(ConnectionContext* context)
{
    atomic_store(&context->closing, true);

    // Shut down the socket to signal the receive thread to stop
    if (shutdown(context->socketfd, SHUT_RDWR) == -1) {
        return CREATE_ERRNO(ERR_NET_FAILURE, "shutdown() failure");
    }

    // Waits 2ms, in case there is any thread receiving
    usleep(20000);

    int32_t fd = context->socketfd;
    free(context);

    if (close(fd) == -1)
        return CREATE_ERRNO(ERR_CLOSE_FD, "Error while closing ConnectionContext socket file descriptor");

    return CREATE_ERR_OK;
}
void net_shutdown()
{
}

Error* net_get_ip(
    ConnectionContext* context,
    char* ip_buffer,
    size_t ip_buffer_size)
{
    // Convert the IP address from binary to text form
    const char* result = inet_ntop(AF_INET, &(context->addr.sin_addr), ip_buffer, ip_buffer_size);
    if (result == NULL)
        return CREATE_ERR(ERR_NET_FAILURE, "Error while converting IP to human-readable form");

    return CREATE_ERR_OK;
}

bool net_verify_certificate(ConnectionContext*)
{
    return false;
}
