#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <unistd.h>
#include <thread_pool.h>
#include <signal.h>
#include "server.h"
#include "messages/message.h"
#include "messages/message_types.h"
#include "net/net_communication.h"
#include "net/net_stream.h"
#include "broadcast_message.h"
#include "client_handler.h"

/// @brief Allocates memory for server and initializes fields
Server* server_create(uint16_t port, uint32_t max_client_count);

/// @brief Frees server and related memory
Error* server_free(Server* server);

/// @brief Initializes the server socket
Error* server_init_network(Server* server);

/// @brief Accepts client connections, and create a handle thread
/// for each new connection.
Error* server_accept_clients(Server* server);

static Server* server;

void handle_sigint(int sig)
{
    Error* err = server_free(server);
    print_error(err);
    exit(0);
}

int main(int argc, char** argv)
{

    uint16_t port = DEFAULT_STATUS_PORT;
    uint8_t max_client_count = 10;

    // Loads arguments
    int i = 0;
    while (++i < argc - 1) {
        char* parameter = argv[i++];

        if (!strcmp(parameter, "--port")) {
            char* port_str = argv[i];
            port = atoi(port_str);
        }

        if (!strcmp(parameter, "--max_clients")) {
            char* max_clients_str = argv[i];
            max_client_count = atoi(max_clients_str);
        }

        else
            printf("Unrecognized parameter \"%s\"", parameter);
    }

    // Set up the signal handler for SIGINT (CTRL+C)
    signal(SIGINT, handle_sigint);

    // Creates server with the input parameters
    server = server_create(port, max_client_count);

    Error* err = server_init_network(server);
    ASSERT_NET_ERROR(err);

    err = server_accept_clients(server);
    ASSERT_NET_ERROR(err);

    server_free(server);
    ASSERT_NET_ERROR(err);

    return EXIT_SUCCESS;
}

Server* server_create(uint16_t port, uint32_t max_client_count)
{
    Server* server = (Server*)malloc(sizeof(Server));
    memset(server, 0, sizeof(Server));

    server->port = port;
    server->max_client_count = max_client_count;
    pthread_mutex_init(&server->broadcast_mutex, NULL);

    // Initializes client list
    server->client_list = client_list_create();
    pthread_mutex_init(&server->client_list_mutex, NULL);

    server->client_thread_pool = thpool_create(max_client_count);
    return server;
}

Error* server_free(Server* server)
{
    printf("Freeing server memory\n");
    Error* err = net_close(server->context);
    printf("    - Context closed\n");
    thpool_destroy(server->client_thread_pool);
    printf("    - Thpool destroyed\n");
    client_list_destroy(server->client_list);
    printf("    - Client list destroyed\n");
    pthread_mutex_destroy(&server->client_list_mutex);
    pthread_mutex_destroy(&server->broadcast_mutex);
    printf("    - Mutexes destroyed\n");
    net_shutdown();
    printf("    - Network shutdown\n");
    return err;
}

Error* server_init_network(Server* server)
{

    // Initializes network module.
    net_init();
    Error* err;

#ifdef THEY_CHAT_SSL
    printf("Loading SSL certificate and key\n");
    // Certificate and private key paths
    const char* home_path = getenv("HOME");
    char cert_file[128];
    char key_file[128];
    sprintf(cert_file, "%s/.ssl/TheyChat/certificate.pem", home_path);
    sprintf(key_file, "%s/.ssl/TheyChat/private.key", home_path);

    printf("- Certificate path: %s\n", cert_file);
    printf("- Private key path: %s\n", key_file);
    // Initializes socket with certificates
    err = net_server_create_socket(cert_file, key_file, server->port, &server->context);

#else
    // Not using SSL, certificates not needed
    err = net_server_create_socket(NULL, NULL, server->port, &server->context);
#endif
    return err;
}

Error* server_accept_clients(Server* server)
{

    // Sets up listen
    Error* err = net_listen(server->context, 5);
    if (IS_NET_ERROR(err))
        return err;

    printf("Listening on port %i. Waiting for client connections...\n", server->port);

    // Message sent to client in case there aren't any free
    // threads, and stays in client queue
    Message client_on_queue_msg = create_client_on_queue();

    while (true) {

        // Accepts client connections
        ConnectionContext* client_status_context;
        err = net_accept_connection(server->context, &client_status_context);

        if (IS_NET_ERROR(err))
            break;

        printf("Accepted client status connection\n");

        ConnectionContext* client_cmd_context;
        err = net_accept_connection(server->context, &client_cmd_context);

        if (IS_NET_ERROR(err))
            break;

        printf("Accepted client command connection\n");

        // Registers client
        pthread_mutex_lock(&server->client_list_mutex);
        Client* client = client_list_add(server->client_list);
        pthread_mutex_unlock(&server->client_list_mutex);

        // Initializes client network data
        init_client_network(client, client_status_context, client_cmd_context);

        // Tells client that all threads are busy, and it's on queue
        if (thpool_all_threads_busy(server->client_thread_pool))
            send_message((Message*)&client_on_queue_msg, &client->status_connection);

        // Creates handler data and queues a new task
        ClientHandlerData* handler_data = (ClientHandlerData*)malloc(sizeof(ClientHandlerData));
        handler_data->server = server;
        handler_data->client = client;
        thpool_submit(server->client_thread_pool, (thread_task_t)handle_client_task, handler_data);
    }
    return err;
}