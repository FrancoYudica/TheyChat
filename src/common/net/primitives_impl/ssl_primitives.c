
#include "net/net_primitives.h"
#include <memory.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

struct ConnectionContext {
    SSL_CTX* ctx;
    SSL* ssl;
    int32_t socket;
    BIO* bio;
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

// static void ssl_connect(
//     ConnectionContext* ctx,
//     const char* host,
//     uint16_t port)
// {
//     ctx->ssl = SSL_new(ctx->ctx);
//     BIO* bio = BIO_new_ssl_connect(ctx->ctx);
//     BIO_get_ssl(bio, &ctx->ssl);
//     SSL_set_mode(ctx->ssl, SSL_MODE_AUTO_RETRY);

//     char hostname[1024];
//     snprintf(hostname, sizeof(hostname), "%s:%i", host, port);
//     BIO_set_conn_hostname(bio, hostname);

//     // Set hostname for TLS SNI extension
//     SSL_set_tlsext_host_name(ctx->ssl, host);

//     if (BIO_do_connect(bio) <= 0) {
//         report_and_exit("BIO_do_connect");
//     }
//     ctx->socket = BIO_get_fd(bio, NULL);
// }

static void ssl_connect(
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
    if (BIO_do_connect(ctx->bio) <= 0) {
        report_and_exit("BIO_do_connect");
    }

    // Verify server certificate
    if (SSL_get_verify_result(ctx->ssl) != X509_V_OK) {
        printf("Couldn't verify certificate...\n");
    }

    // Get file descriptor for socket
    ctx->socket = BIO_get_fd(ctx->bio, NULL);
    BIO_set_fd(ctx->bio, -1, BIO_NOCLOSE);
}

void net_init()
{
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();
}

ConnectionContext* net_server_create_socket(
    const char* cert_file,
    const char* key_file,
    uint32_t port)
{
    ConnectionContext* context = (ConnectionContext*)malloc(sizeof(ConnectionContext));
    memset(context, 0, sizeof(ConnectionContext));
    // Creates SSL context with the latest version possible
    const SSL_METHOD* method = SSLv23_server_method();
    context->ctx = SSL_CTX_new(method);
    if (!context->ctx) {
        report_and_exit("Unable to create SSL context");
    }

    // Configures certificate and key
    configure_context(context, cert_file, key_file);

    // Creates TCP socket
    context->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (context->socket < 0) {
        report_and_exit("socket");
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    // Binds to port
    if (bind(context->socket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        report_and_exit("bind");
    }
    return context;
}

ConnectionContext* net_client_create_socket(
    uint32_t server_port,
    const char* server_ip)
{
    ConnectionContext* context = (ConnectionContext*)malloc(sizeof(ConnectionContext));
    memset(context, 0, sizeof(ConnectionContext));

    const SSL_METHOD* method = SSLv23_client_method();
    context->ctx = SSL_CTX_new(method);
    if (!context->ctx) {
        report_and_exit("Unable to create SSL context");
    }
    ssl_connect(context, server_ip, server_port);
    return context;
}

void net_listen(ConnectionContext* context, uint32_t n)
{
    if (listen(context->socket, n) < 0) {
        report_and_exit("listen");
    }
}
ErrorCode net_accept_connection(
    ConnectionContext* server_context,
    ConnectionContext** client_context_ref)
{

    ConnectionContext* client_context = malloc(sizeof(ConnectionContext));
    *client_context_ref = client_context;

    // Accepts connection with usual socket
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    client_context->socket = accept(server_context->socket, (struct sockaddr*)&client_addr, &client_len);
    if (client_context->socket < 0) {
        return ERR_NET_ACCEPT_FAIL;
    }

    // Sets context and file descriptor
    client_context->ctx = server_context->ctx;
    client_context->ssl = SSL_new(client_context->ctx);
    SSL_set_fd(client_context->ssl, client_context->socket);

    // Accepts with SSL
    if (SSL_accept(client_context->ssl) <= 0) {
        return ERR_NET_ACCEPT_FAIL;
    }
    return ERR_NET_OK;
}

ErrorCode net_send(
    ConnectionContext* context,
    const uint8_t* buffer,
    uint32_t size,
    uint32_t* bytes_sent)
{
    int bytes = SSL_write(context->ssl, buffer, size);
    if (bytes <= 0)
        return ERR_SEND_FAIL;

    if (bytes_sent != NULL)
        *bytes_sent = bytes;

    return ERR_NET_OK;
}

ErrorCode net_receive(
    ConnectionContext* context,
    uint8_t* buffer,
    uint32_t size,
    uint32_t* bytes_read)
{
    int read = SSL_read(context->ssl, buffer, size);
    if (read == -1)
        return ERR_RECEIVE_FAIL;
    if (read == 0)
        return ERR_PEER_DISCONNECTED;

    if (bytes_read != NULL)
        *bytes_read = read;

    return ERR_NET_OK;
}

void net_close(ConnectionContext* context)
{
    SSL_shutdown(context->ssl);
    SSL_free(context->ssl);
    close(context->socket);
    free(context);
}

void net_shutdown()
{
    EVP_cleanup();
}

ErrorCode net_get_ip(ConnectionContext* context, char* ip_buffer, size_t ip_buffer_size)
{
    if (context->ssl == NULL || ip_buffer == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    // Get the underlying file descriptor from the SSL object
    int fd = SSL_get_fd(context->ssl);
    if (fd < 0) {
        perror("SSL_get_fd");
        return ERR_NET_FAILURE;
    }

    // Get the peer address
    struct sockaddr_in peer_addr;
    socklen_t peer_addr_len = sizeof(peer_addr);
    if (getpeername(fd, (struct sockaddr*)&peer_addr, &peer_addr_len) < 0) {
        perror("getpeername");
        return ERR_NET_FAILURE;
    }

    // Convert the IP address to a human-readable form
    const char* result = inet_ntop(AF_INET, &peer_addr.sin_addr, ip_buffer, ip_buffer_size);
    if (result == NULL) {
        perror("inet_ntop");
        return ERR_NET_FAILURE;
    }

    return ERR_NET_OK;
}