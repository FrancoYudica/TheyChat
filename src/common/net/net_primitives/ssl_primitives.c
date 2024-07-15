
#include "net/net_primitives.h"
#include "net/socket.h"
#include <memory.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <stdatomic.h>

struct ConnectionContext {
    SSL_CTX* ctx;
    SSL* ssl;
    int32_t socketfd;
    BIO* bio;

    atomic_bool closing;
};

static void report_and_exit(const char* msg)
{
    perror(msg);
    ERR_print_errors_fp(stderr);
    exit(-1);
}

static void configure_context(
    ConnectionContext* ctx,
    const char* cert_file,
    const char* key_file)
{
    if (SSL_CTX_use_certificate_file(ctx->ctx, cert_file, SSL_FILETYPE_PEM) <= 0) {
        report_and_exit("SSL_CTX_use_certificate_file");
    }

    if (SSL_CTX_use_PrivateKey_file(ctx->ctx, key_file, SSL_FILETYPE_PEM) <= 0) {
        report_and_exit("SSL_CTX_use_PrivateKey_file");
    }
}

static Error* ssl_connect(
    ConnectionContext* ctx,
    const char* host,
    uint16_t port)
{
    // Create SSL object
    ctx->ssl = SSL_new(ctx->ctx);

    // Set hostname for TLS SNI extension
    SSL_set_tlsext_host_name(ctx->ssl, host);

    // Create BIO object for SSL connection
    ctx->bio = BIO_new_ssl_connect(ctx->ctx);
    BIO_get_ssl(ctx->bio, &ctx->ssl);
    SSL_set_mode(ctx->ssl, SSL_MODE_AUTO_RETRY);

    // Set connection hostname and port
    char hostname[1024];
    snprintf(hostname, sizeof(hostname), "%s:%i", host, port);
    BIO_set_conn_hostname(ctx->bio, hostname);

    // Perform SSL handshake
    if (BIO_do_connect(ctx->bio) <= 0)
        return CREATE_ERRNO(ERR_NET_UNABLE_TO_CONNECT, "Unable to perform SSL handshake");

    // Get file descriptor for socketfd
    ctx->socketfd = BIO_get_fd(ctx->bio, NULL);
    BIO_set_fd(ctx->bio, -1, BIO_NOCLOSE);

    return CREATE_ERR_OK;
}

static void init_connection_context(ConnectionContext* context)
{
    memset(context, 0, sizeof(ConnectionContext));
    context->socketfd = 0;
    context->closing = false;
    atomic_store(&context->closing, false);
}

void net_init()
{
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();
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
    context->ssl = NULL;

    // Creates SSL context with the latest version possible
    const SSL_METHOD* method = SSLv23_server_method();
    context->ctx = SSL_CTX_new(method);
    if (!context->ctx)
        return CREATE_ERRNO(ERR_NET_FAILURE, "Unable to create SSL context");

    // Configures certificate and key
    configure_context(context, cert_file, key_file);

    // Creates TCP socketfd
    context->socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (context->socketfd < 0)
        return CREATE_ERRNO(ERR_NET_SOCKET_CREATION_FAILED, "Unable to create socketfd");

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    // Binds to port
    if (bind(context->socketfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        return CREATE_ERRNO(ERR_NET_FAILURE, "Unable to bind socketfd");

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

    const SSL_METHOD* method = SSLv23_client_method();
    context->ctx = SSL_CTX_new(method);
    if (!context->ctx)
        return CREATE_ERRNO(ERR_NET_FAILURE, "Unable to create SSL context");

    return ssl_connect(context, server_ip, server_port);
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
    ConnectionContext* server_context,
    ConnectionContext** client_context_ref)
{
    ConnectionContext* client_context = malloc(sizeof(ConnectionContext));
    *client_context_ref = client_context;
    init_connection_context(client_context);

    // Accepts connection with usual socketfd
    struct sockaddr_in client_addr;
    uint32_t client_len = sizeof(client_addr);
    client_context->socketfd = accept(server_context->socketfd, (struct sockaddr*)&client_addr, &client_len);

    if (client_context->socketfd == -1)
        return CREATE_ERRNO(ERR_NET_FAILURE, "Accept fail");

    // Sets context and file descriptor
    client_context->ctx = server_context->ctx;
    client_context->ssl = SSL_new(client_context->ctx);
    SSL_set_fd(client_context->ssl, client_context->socketfd);

    // Accepts with SSL
    if (SSL_accept(client_context->ssl) <= 0)
        return CREATE_ERRNO(ERR_NET_FAILURE, "SSL accept fail");

    return CREATE_ERR_OK;
}

Error* net_send(
    ConnectionContext* context,
    const uint8_t* buffer,
    uint32_t size,
    uint32_t* bytes_sent)
{
    if (atomic_load(&context->closing))
        return CREATE_ERR(ERR_NET_CONNECTION_CLOSED, "Trying to send when connection is closed");

    int bytes = SSL_write(context->ssl, buffer, size);
    if (bytes <= 0)
        return CREATE_ERRNO(ERR_NET_FAILURE, "SSL_write failure");

    if (bytes_sent != NULL)
        *bytes_sent = bytes;

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

            int read = SSL_read(context->ssl, buffer, size);
            if (read == -1)
                return CREATE_ERRNO(ERR_NET_FAILURE, "`SSL_read` failure");

            if (read == 0)
                return CREATE_ERR(ERR_NET_PEER_DISCONNECTED, "Peer disconnected");

            if (bytes_read != NULL)
                *bytes_read = read;

            return CREATE_ERR_OK;
        }
    }

    return CREATE_ERR(ERR_NET_FAILURE, "Unexpected error in select()");
}

Error* net_close(ConnectionContext* context)
{
    atomic_store(&context->closing, true);

    // Shut down the socket to signal the receive thread to stop
    if (context->ssl != NULL && SSL_shutdown(context->ssl) != 0)
        return CREATE_ERRNO(ERR_NET_FAILURE, "SSL_shutdown() failure");

    // Waits 2ms, in case there is any thread receiving
    usleep(20000);

    if (context->ssl != NULL)
        SSL_free(context->ssl);

    int32_t fd = context->socketfd;
    free(context);

    if (close(fd) == -1)
        return CREATE_ERRNO(ERR_CLOSE_FD, "Error while closing ConnectionContext socket file descriptor");

    return CREATE_ERR_OK;
}

void net_shutdown()
{
    EVP_cleanup();
}

Error* net_get_ip(ConnectionContext* context, char* ip_buffer, size_t ip_buffer_size)
{
    // Get the underlying file descriptor from the SSL object
    int32_t fd = SSL_get_fd(context->ssl);
    if (fd == -1)
        return CREATE_ERRNO(ERR_CLOSE_FD, "Error while getting context socket file descriptor");

    // Get the peer address
    struct sockaddr_in peer_addr;
    uint32_t peer_addr_len = sizeof(peer_addr);

    if (getpeername(fd, (struct sockaddr*)&peer_addr, &peer_addr_len) < 0)
        return CREATE_ERRNO(ERR_NET_FAILURE, "Error while getting peer address");

    // Convert the IP address to a human-readable form
    const char* result = inet_ntop(AF_INET, &peer_addr.sin_addr, ip_buffer, ip_buffer_size);
    if (result == NULL)
        return CREATE_ERR(ERR_NET_FAILURE, "Error while converting IP to human-readable form");

    return CREATE_ERR_OK;
}

bool net_verify_certificate(ConnectionContext* context)
{
    // Verify server certificate
    return SSL_get_verify_result(context->ssl) != X509_V_OK;
}