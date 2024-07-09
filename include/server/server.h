#ifndef __SERVER_H__
#define __SERVER_H__
#include "client_list.h"
#include "pthread.h"
#include "net/net_primitives.h"
#include <thread_pool.h>

typedef struct
{
    ClientList* client_list;
    uint16_t port;
    uint32_t max_client_count;

    thpool_t* client_thread_pool;

    thpool_t* task_thread_pool;

    ConnectionContext* context;

    /// @brief Mutex used exclusively to block other
    /// thread when modifying client list
    pthread_mutex_t client_list_mutex;

    /// @brief Mutex used exclusively to block other
    /// threads when sending messages in broadcast
    pthread_mutex_t broadcast_mutex;
} Server;

/// @brief Notifies all clients the connected clients
void server_client_count_update(Server* server);

#endif