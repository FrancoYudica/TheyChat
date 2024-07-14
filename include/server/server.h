#ifndef __SERVER_H__
#define __SERVER_H__
#include "client_list.h"
#include "shared_file_list.h"
#include "pthread.h"
#include "net/net_primitives.h"
#include <thread_pool.h>

typedef struct
{
    /// @brief Data structure that stores all the clients
    ClientList* client_list;

    /// @brief Data structure that stores all the uploaded and
    /// downloadable files
    SharedFileList* shared_file_list;

    uint16_t status_port, task_port;

    uint32_t max_client_count;

    ConnectionContext *status_context, *task_context;

    /// @brief Thread pool used to handle clients
    thpool_t* client_thread_pool;

    /// @brief Thread pool used to handle tasks
    thpool_t* task_thread_pool;

    /// @brief Mutex used exclusively to block other
    /// thread when modifying client list
    pthread_mutex_t client_list_mutex;

    /// @brief Mutex used exclusively to block other
    /// thread when modifying shared file list
    pthread_mutex_t shared_file_list_mutex;

    /// @brief Mutex used exclusively to block other
    /// threads when sending messages in broadcast
    pthread_mutex_t broadcast_mutex;
} Server;

Server* get_server();

Error* server_init(
    uint16_t status_port,
    uint16_t task_port,
    uint32_t max_client_count);

Error* server_run();

Error* server_free();

bool server_is_running();

/// @brief Notifies all clients the connected clients
void server_client_count_update();

Error* server_remove_shared_file(uint32_t file_id);

void server_remove_client_files(
    uint32_t client_id,
    uint32_t* removed_count);

#endif