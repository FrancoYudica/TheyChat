#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <stdint.h>
#include <arpa/inet.h>
#include "net/net_error.h"

// Structure to hold connection context information
typedef struct {
    /// @brief Socket file descriptor
    uint32_t socketfd;

    /// @brief Socket address structure
    struct sockaddr_in addr;
} ConnectionContext;

/**
 * @brief Initializes a server socket and binds it to the specified port.
 *
 * @param port The port number to bind the server socket to.
 * @param context Pointer to the ConnectionContext structure to initialize.
 */
void net_server_init_socket(uint32_t port, ConnectionContext* context);

/**
 * @brief Creates a client socket and sets up its connection context.
 *
 * @param server_port The port number of the server to connect to.
 * @param server_ip The IP address of the server to connect to.
 * @param context Pointer to the ConnectionContext structure to initialize.
 */
void net_client_init_socket(uint32_t server_port, const char* server_ip, ConnectionContext* context);

/**
 * @brief Puts the server socket into listening mode.
 *
 * @param context Pointer to the ConnectionContext structure of the server.
 * @param n The maximum number of pending connections the server can queue.
 */
void net_listen(ConnectionContext* context, uint32_t n);

/**
 * @brief Accepts a new connection on the server socket.
 *
 * @param context Pointer to the ConnectionContext structure of the server.
 * @param client_context Pointer to the ConnectionContext structure to hold the new client connection.
 * @return ErrorCode Error code indicating success or failure.
 */
ErrorCode net_accept_connection(ConnectionContext* context, ConnectionContext* client_context);

/**
 * @brief Connects the client socket to the server.
 *
 * @param context Pointer to the ConnectionContext structure of the client.
 * @return ErrorCode Error code indicating success or failure.
 */
ErrorCode net_connect_to_server(ConnectionContext* context);

/**
 * @brief Sends data over the connection.
 *
 * @param context Pointer to the ConnectionContext structure of the connection.
 * @param buffer The buffer containing the data to send.
 * @param len The length of the data to send.
 * @param bytes_sent Pointer to a variable to hold the number of bytes actually sent.
 * @return ErrorCode Error code indicating success or failure.
 */
ErrorCode net_send(ConnectionContext* context, const uint8_t* buffer, ssize_t len, uint32_t* bytes_sent);

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
 * @brief Cleans up the connection by closing the socket.
 *
 * @param context Pointer to the ConnectionContext structure of the connection to clean up.
 */
void net_close(ConnectionContext* context);

#endif
