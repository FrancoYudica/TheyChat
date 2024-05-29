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

    struct sockaddr_in server_addr;

    // Create TCP socket
    server.sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server.sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    memset(&server_addr, 0, sizeof(server_addr));

    // IPv4
    server_addr.sin_family = AF_INET;

    // Sets the server ip to local ip
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(server.port);

    // Bind socket to the server address
    if (bind(server.sockfd, (const struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Sets socket in passive mode, allowing future connections
    // in the previously bound port
    if (listen(server.sockfd, 1) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("SERVER: Listening. Waiting for client connections...\n");
    socklen_t client_len = sizeof(struct sockaddr_in);

    thpool_t* thpool = thpool_create(2);

    // Initializes client list
    server.client_list = client_list_create();

    // Message sent to client in case there aren't any free
    // threads, and stays in client queue
    Bytes128Msg* client_on_queue_msg = create_client_on_queue();

    while (true) {
        struct sockaddr_in client_address;

        // Accepts client connection
        int32_t client_sock_fd = accept(
            server.sockfd,
            (struct sockaddr*)&client_address,
            &client_len);

        if (client_sock_fd == -1) {
            perror("accept failed");
            break;
        }

        // Registers client
        Client* client = client_list_add(server.client_list);

        // Initializes client network data
        init_client_network(client, &client_address, client_sock_fd);

        // Tells client that all threads are busy, and it's on queue
        if (thpool_all_threads_busy(thpool))
            send_message((Message*)client_on_queue_msg, client_sock_fd);

        // Creates handler data and queues a new task
        ClientHandlerData* handler_data = (ClientHandlerData*)malloc(sizeof(ClientHandlerData));
        handler_data->server = &server;
        handler_data->client = client;
        thpool_submit(thpool, (thread_task_t)handle_client_task, handler_data);
    }

    if (close(server.sockfd) == -1)
        perror("close");

    return EXIT_SUCCESS;
}