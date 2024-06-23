#ifndef __NETWORK_PRIMITIVES_H__
#define __NETWORK_PRIMITIVES_H__

#include <stdint.h>
#include <arpa/inet.h>
#include "they_chat_error.h"

// Forward declaration of the ConnectionContext structure
typedef struct ConnectionContext ConnectionContext;

/// Initializes the network context.
void net_init();

/**
 * Creates a server socket and binds it to a port.
 *
 * @param cert_file Path to the server's certificate file.
 * @param key_file Path to the server's key file.
 * @param port Port number to bind the server socket.
 * @return Error* Error object or NULL on success.
 */
Error* net_server_create_socket(
    const char* cert_file,
    const char* key_file,
    uint32_t port,
    ConnectionContext** context_ref);

/**
 * Creates a client socket.
 *
 * @param server_port Port number of the server to connect to.
 * @param server_ip IP address of the server to connect to.
 * @return Error* Error object or NULL on success.
 */
Error* net_client_create_socket(
    uint32_t server_port,
    const char* server_ip,
    ConnectionContext** context);

/**
 * Puts the server socket into listening mode.
 *
 * @param context Server socket's ConnectionContext.
 * @param n Maximum number of pending connections.
 * @return Error* Error object or NULL on success.
 */
Error* net_listen(
    ConnectionContext* context,
    uint32_t n);

/**
 * Accepts a new connection on the server socket.
 *
 * @param server_context Server socket's ConnectionContext.
 * @param client_context_ref Pointer to store the new client connection's ConnectionContext.
 * @return Error* Error object or NULL on success.
 */
Error* net_accept_connection(
    ConnectionContext* server_context,
    ConnectionContext** client_context_ref);

/**
 * Sends data over the connection.
 *
 * @param context ConnectionContext of the connection.
 * @param buffer Data to send.
 * @param len Length of the data.
 * @param bytes_sent Pointer to store the number of bytes sent.
 * @return Error* Error object or NULL on success.
 */
Error* net_send(
    ConnectionContext* context,
    const uint8_t* buffer,
    uint32_t len,
    uint32_t* bytes_sent);

/**
 * Receives data from the connection.
 *
 * @param context ConnectionContext of the connection.
 * @param buffer Buffer to store received data.
 * @param size Maximum size of data to receive.
 * @param bytes_read Pointer to store the number of bytes read.
 * @return Error* Error object or NULL on success.
 */
Error* net_receive(
    ConnectionContext* context,
    uint8_t* buffer,
    uint32_t size,
    uint32_t* bytes_read);

/**
 * Closes the connection.
 *
 * @param context ConnectionContext of the connection to close.
 * @return Error* Error object or NULL on success.
 */
Error* net_close(ConnectionContext* context);

/// Shuts down the network context.
void net_shutdown();

/**
 * Retrieves the IP address from a ConnectionContext as a string.
 *
 * @param context ConnectionContext structure.
 * @param ip_buffer Buffer to store the IP address string.
 * @param ip_buffer_size Size of the buffer.
 * @return Error* Error object or NULL on success.
 */
Error* net_get_ip(
    ConnectionContext* context,
    char* ip_buffer,
    size_t ip_buffer_size);

#endif
