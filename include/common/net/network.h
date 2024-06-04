#ifndef __NETWORK_H__
#define __NETWORK_H__
#include <stdint.h>
#include <arpa/inet.h>
#include "net/net_error.h"

typedef struct {
    uint32_t socketfd;
    struct sockaddr_in addr;
} ConnectionContext;

void net_server_init_socket(uint32_t port, ConnectionContext* context);

ConnectionContext net_client_create_socket(uint32_t server_port, const char* server_ip);

void net_listen(ConnectionContext* context, uint32_t n);

ErrorCode net_accept_connection(ConnectionContext* context, ConnectionContext* client_context);

ErrorCode net_connect_to_server(ConnectionContext* context);

ErrorCode net_send(ConnectionContext* context, const uint8_t* buffer, ssize_t len, uint32_t* bytes_sent);

ErrorCode net_receive(ConnectionContext* context, uint8_t* buffer, uint32_t size, uint32_t* bytes_read);

void net_cleanup(ConnectionContext* context);

#endif