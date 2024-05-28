#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "messages/message.h"
#include "messages/message_types.h"
#include "net/serialization/net_message_serializer.h"
#include "net/net_communication.h"
#include "pthread.h"

uint32_t server_port = 8000;
char *server_ip = "127.0.0.1";
NetworkStream network_stream;

struct
{
    uint32_t sockfd;
    char username[128];
} data;



void connect_to_server()
{
    // Create TCP socket
    data.sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (data.sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    socklen_t server_len = sizeof(struct sockaddr_in);

    // Initialize server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    // Establishes connection with server
    int32_t connection_status = connect(data.sockfd, (const struct sockaddr*)&server_addr, sizeof(const struct sockaddr_in));
    if (connection_status == -1)
    {
        perror("Unable to connect");
        exit(EXIT_FAILURE);
    }
}

ErrorCode wait_in_queue()
{

    // Waits for connection
    while (true)
    {
        Message *server_message;
        ErrorCode status = wait_for_message(&network_stream, data.sockfd, &server_message);
        assert(!IS_NET_ERROR(status));

        // Client connected successfully;
        if (server_message->header.type == MSGT_CLIENT_CONNECTED)
        {
            printf("%s\n", ((Bytes128Msg*)server_message)->bytes);
            free(server_message);
            break;
        }

        // Client in queue
        if (server_message->header.type == MSGT_CLIENT_ON_QUEUE)
            printf("%s\n", ((Bytes128Msg*)server_message)->bytes);
        else
            printf("Received unexpected file type %i\n", server_message->header.type);

        free(server_message);
    }
    return ERR_NET_OK;

}

ErrorCode chat_login()
{
    while (true)
    {
        // Gets username
        printf("%s", "Enter username: ");
        fgets(data.username, sizeof(data.username), stdin);

        // Replaces '\n'
        data.username[strlen(data.username) - 1] = '\0';

        // Sends message telling that the username
        UserLoginMsg *login_msg = create_user_login_msg(data.username);
        send_message((const Message*)login_msg, data.sockfd);
        free(login_msg);

        StatusMsg *status_message;
        ErrorCode status = wait_for_message_type(&network_stream, data.sockfd, (Message**)&status_message, MSGT_STATUS);

        if (IS_NET_ERROR(status))
        {
            printf("Didn't receive StatusMsg\n");
            return status;
        }

        print_message((Message*)status_message);

        if (status_message->status == STATUS_MSG_FAILURE)
        {
            printf("%s\n", status_message->text);
        }
        else
        {
            break;
        }

    }
    return ERR_NET_OK;
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
            server_port = atoi(port_str);
        }

        else if (!strcmp(parameter, "--server_ip"))
        {
            server_ip = argv[i];
        }

        else
        {
            printf("Unrecognized parameters (%s, %s)", parameter, argv[i]);
        }
    }

    connect_to_server();

    init_net_stream(&network_stream);

    ASSERT_NET_ERROR(wait_in_queue());
    ASSERT_NET_ERROR(chat_login());


    Message *message = (Message*)create_user_chat_msg("Received message!", data.username);

    while(true)
    {
        Message *server_message;
        ErrorCode status = wait_for_message(&network_stream, data.sockfd, &server_message);

        if (IS_NET_ERROR(status))
        {
            if (status == ERR_PEER_DISCONNECTED)
                printf("Server disconnected\n");
            else   
                printf("Error code: %i\n", status);

            break;
        }
        print_message(server_message);
        free(server_message);
        
        send_message((const Message*)message, data.sockfd);
        sleep(1);
    }
    free(message);
    printf("Closing socket fd %d\n", data.sockfd);

    if (close(data.sockfd) == -1)
        perror("close");

    return EXIT_SUCCESS;
}