#include <stdlib.h>
#include <stddef.h>
#include <thread_pool.h>
#include "message.h"
#include "message_types.h"
#include "net/net_communication.h"
#include "net/net_stream.h"
#include "server/client_list.h"
#include <unistd.h>

uint32_t port = 8000;

void client_disconnected(const Client *client)
{
    printf("Client %i disconnected\n", client->id);
}

void handle_client_task(void *arg)
{
    Client *client = (Client*)(arg);
    
    NetworkStream stream;
    init_net_stream(&stream);

    UserChatMsg message;
    init_user_chat_msg(&message, "Welcome to TheyChat!", "SERVER");
    printf("Handling client %d\n", client->sockfd);

    while(true)
    {
        net_status_t send_status = send_message((Message*)&message, client->sockfd);

        if (IS_NET_ERROR(send_status))
        {
            printf("NETWORK ERROR type %i while handling client %d\n", send_status, client->id);
            break;
        }

        // Waits for any message
        Message *msg;
        net_status_t receive_status = wait_for_message(&stream, client->sockfd, &msg);

        if (IS_NET_ERROR(receive_status))
        {
            if (receive_status != NET_ERROR_PEER_DISCONNECTED)
                printf("NETWORK ERROR type %i while handling client %d\n", send_status, client->id);

            break;
        }
        // Outputs client message
        print_message(msg);

        free(msg);
    }

    client_disconnected(client);
}


int main(int argc, char** argv)
{

    // Loads arguments
    int i = 0;
    while (++i < argc - 1)
    {
        char* parameter = argv[i++];

        if (!strcmp(parameter, "--port"))
        {
            char* port_str = argv[i];
            port = atoi(port_str);
        }

        else
            printf("Unrecognized parameter \"%s\"", parameter);
    }

    struct sockaddr_in server_addr;

    // Create TCP socket
    uint32_t sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;   // Sets the server ip to local ip
    server_addr.sin_port = htons(port);

    // Bind socket to the server address
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Sets socket in passive mode, allowing future connections
    // in the previously bound port
    if (listen(sockfd, 1) == -1)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("SERVER: Listening. Waiting for client connections...\n");
    socklen_t client_len = sizeof(struct sockaddr_in);

    thpool_t *thpool = thpool_create(32);
    ClientList *client_list = client_list_create();

    UserBaseMsg connected_msg;
    init_user_chat_msg(&connected_msg, "Connected to the server. Waiting for thread handle...", "SERVER");

    while(true)
    {
        struct sockaddr_in client_address;
        // Accepts connection
        int32_t client_sock_fd = accept(
            sockfd, 
            (struct sockaddr*)&client_address, 
            &client_len
        );
    
        if (client_sock_fd == -1) {
            perror("recvfrom failed");
            close(sockfd);
            continue;
        }

        // Registers client
        Client *client = client_list_add(client_list);

        // Initializes client network data
        init_client_network(client, &client_address, client_sock_fd);

        // Tells the client it's connected to the server
        send_message(&connected_msg, client_sock_fd);

        // Queues a new task
        thpool_submit(thpool, handle_client_task, client);
    }

    if (close(sockfd) == -1)
        perror("close");

    return EXIT_SUCCESS;
}