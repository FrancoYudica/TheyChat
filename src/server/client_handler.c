#include "client_handler.h"
#include "client.h"
#include "net/net_stream.h"
#include "net/net_communication.h"

#include "message_types.h"

static void handle_login(NetworkStream *stream, Client *client);
static void client_disconnected(Client *client);

void handle_client_task(void *arg)
{
    Client *client = (Client*)(arg);
    printf("Handling client %d\n", client->sockfd);
    
    NetworkStream stream;
    init_net_stream(&stream);

    {
        // Tells client that it's connected
        Bytes128Msg *connected_message = create_client_connected();
        net_status_t status = send_message((const Message*)connected_message, client->sockfd);
        ASSERT_NET_ERROR(status);
        free(connected_message);
    }

    handle_login(&stream, client);

    UserChatMsg *msg = create_user_chat_msg("Hey!", "SERVER");

    while(true)
    {
        net_status_t send_status = send_message((const Message*)msg, client->sockfd);
        
        if (IS_NET_ERROR(send_status))
        {
            if (send_status != NET_ERROR_PEER_DISCONNECTED)
                printf("NETWORK ERROR type %i while handling client %d\n", send_status, client->id);
            break;
        }

        // Waits for any message
        Message *client_msg;
        net_status_t receive_status = wait_for_message(&stream, client->sockfd, &client_msg);

        if (IS_NET_ERROR(receive_status))
        {
            if (receive_status != NET_ERROR_PEER_DISCONNECTED)
                printf("NETWORK ERROR type %i while handling client %d\n", receive_status, client->id);

            break;
        }
        // Outputs client message
        print_message(client_msg);
        free(client_msg);
    }
    free(msg);
    client_disconnected(client);
}

static void handle_login(NetworkStream *stream, Client *client)
{
    // Waits for login
    UserLoginMsg *user_login_msg;
    wait_for_message_type(stream, client->sockfd, (Message**)&user_login_msg, MSGT_USER_LOGIN);
    printf("Client %d logged in with username %s\n", client->id, user_login_msg->user_base.username);
    client->name = user_login_msg->user_base.username;
    free(user_login_msg);
}

static void client_disconnected(Client *client)
{
    printf("Client %i disconnected\n", client->id);
}
