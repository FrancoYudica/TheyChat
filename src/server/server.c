#include "server.h"
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <thread_pool.h>
#include "messages/message.h"
#include "messages/message_types.h"
#include "net/net_communication.h"
#include "net/net_stream.h"
#include "client_handler.h"

int main(int argc, char** argv)
{

    Server server;
    memset(&server, 0, sizeof(Server));
    server.port = 8000;

    // Loads arguments
    int i = 0;
    while (++i < argc - 1) {
        char* parameter = argv[i++];

        if (!strcmp(parameter, "--port")) {
            char* port_str = argv[i];
            server.port = atoi(port_str);
        }

        else
            printf("Unrecognized parameter \"%s\"", parameter);
    }
    net_init();
    thpool_t* thpool = thpool_create(2);

    // Initializes client list
    server.client_list = client_list_create();

    // Initializes socket
    server.context = net_server_create_socket(
        "/home/franco/Certificates/cert.pem",
        "/home/franco/Certificates/key.pem",
        server.port);

    // Sets up listen
    net_listen(server.context, 5);

    printf("SERVER: Listening. Waiting for client connections...\n");

    // Message sent to client in case there aren't any free
    // threads, and stays in client queue
    Bytes128Msg* client_on_queue_msg = create_client_on_queue();

    while (true) {

        // Accepts client connections
        ConnectionContext* client_context;
        ErrorCode err = net_accept_connection(server.context, &client_context);

        if (IS_NET_ERROR(err))
            break;

        // Registers client
        pthread_mutex_lock(&server.client_list_mutex);
        Client* client = client_list_add(server.client_list);
        pthread_mutex_unlock(&server.client_list_mutex);

        // Initializes client network data
        init_client_network(client, client_context);

        // Tells client that all threads are busy, and it's on queue
        if (thpool_all_threads_busy(thpool))
            send_message((Message*)client_on_queue_msg, client->connection_context);

        // Creates handler data and queues a new task
        ClientHandlerData* handler_data = (ClientHandlerData*)malloc(sizeof(ClientHandlerData));
        handler_data->server = &server;
        handler_data->client = client;
        thpool_submit(thpool, (thread_task_t)handle_client_task, handler_data);
    }

    net_close(server.context);
    net_shutdown();

    return EXIT_SUCCESS;
}