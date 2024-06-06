#ifndef __NETWORK_PRIMITIVES_H__
#define __NETWORK_PRIMITIVES_H__

#include <stdint.h>
#include <arpa/inet.h>
#include "net/net_error.h"

// Forward declaration of the ConnectionContext structure
typedef struct ConnectionContext ConnectionContext;

/// @brief Initializes the network context.
void net_init();

/**
 * @brief Creates a server socket and binds it to the specified port.
 *
 * @param cert_file The path to the server's certificate file.
 * @param key_file The path to the server's key file.
 * @param port The port number to bind the server socket to.
 * @return ConnectionContext* Pointer to the ConnectionContext structure representing the server socket.
 */
ConnectionContext* net_server_create_socket(const char* cert_file, const char* key_file, uint32_t port);

/**
 * @brief Creates a client socket and sets up its connection context.
 *
 * @param server_port The port number of the server to connect to.
 * @param server_ip The IP address of the server to connect to.
 * @return ConnectionContext* Pointer to the ConnectionContext structure representing the client socket.
 */
ConnectionContext* net_client_create_socket(uint32_t server_port, const char* server_ip);

/**
 * @brief Puts the server socket into listening mode.
 *
 * @param context Pointer to the ConnectionContext structure of the server socket.
 * @param n The maximum number of pending connections the server can queue.
 */
void net_listen(ConnectionContext* context, uint32_t n);

/**
 * @brief Accepts a new connection on the server socket.
 *
 * @param server_context Pointer to the ConnectionContext structure of the server socket.
 * @param client_context_ref Pointer to a pointer to store the ConnectionContext structure of the new client connection.
 * @return ErrorCode Error code indicating success or failure.
 */
ErrorCode net_accept_connection(ConnectionContext* server_context, ConnectionContext** client_context_ref);

/**
 * @brief Sends data over the connection.
 *
 * @param context Pointer to the ConnectionContext structure of the connection.
 * @param buffer The buffer containing the data to send.
 * @param len The length of the data to send.
 * @param bytes_sent Pointer to a variable to hold the number of bytes actually sent.
 * @return ErrorCode Error code indicating success or failure.
 */
ErrorCode net_send(ConnectionContext* context, const uint8_t* buffer, uint32_t len, uint32_t* bytes_sent);

/**
 * @brief Receives data from the connection.
 *
 * @param context Pointer to the ConnectionContext structure of the connection.
 * @param buffer The buffer to store the received data.
 * @param size The maximum size of data to receive.
 * @param bytes_read Pointer to a variable to hold the number of bytes actually read.
 * @return ErrorCode Error code indicating success or failure.
 */
ErrorCode net_receive(ConnectionContext* context, uint8_t* buffer, uint32_t size, uint32_t* bytes_read);

/**
 * @brief Closes the connection by closing the socket.
 *
 * @param context Pointer to the ConnectionContext structure of the connection to close.
 */
void net_close(ConnectionContext* context);

/// @brief Shuts down the network context.
void net_shutdown();

#endif
