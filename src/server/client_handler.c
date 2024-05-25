#include "client_handler.h"
#include "client.h"
#include "net/net_stream.h"
#include "net/net_communication.h"

#include "message_types.h"

/// @brief Loops until client picks a valid username
static net_status_t handle_login(Client *client);

/// @brief Removes client from client list
static void disconnect_client(Server *server, Client *client);

void handle_client_task(ClientHandlerData *handler_data)
{
    Client *client = handler_data->client;
    Server *server = handler_data->server;

    free(handler_data);

    printf("Handling client %d\n", client->sockfd);
    
    init_net_stream(&client->stream);

    {
        // Tells client that it's connected
        Bytes128Msg *connected_message = create_client_connected();
        net_status_t status = send_message((const Message*)connected_message, client->sockfd);
        ASSERT_NET_ERROR(status);
        free(connected_message);
    }
    printf("Connected message sent\n");

    net_status_t login_status = handle_login(client);

    if (IS_NET_ERROR(login_status))
    {
        disconnect_client(server, client);
        return;
    }

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
        net_status_t receive_status = wait_for_message(&client->stream, client->sockfd, &client_msg);

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
    disconnect_client(server, client);
}

static net_status_t handle_login(Client *client)
{
    while(true)
    {
        UserLoginMsg *user_login_msg;

        // Waits for login
        net_status_t status = wait_for_message_type(&client->stream, client->sockfd, (Message**)&user_login_msg, MSGT_USER_LOGIN);

        if (IS_NET_ERROR(status))
            return status;

        strncpy(client->name, user_login_msg->user_base.username, sizeof(client->name));
        free(user_login_msg);

        // @todo check if name is valid
        bool is_valid = true;

        if (is_valid)
            break;

        // Tell client that the username is invalid
        else
        {
            char text_buffer[128];
            sprintf(text_buffer, "A user named \"%s\" already exists", client->name);
            StatusMsg *status_msg = create_status_msg(STATUS_MSG_FAILURE, text_buffer);
            net_status_t status = send_message((Message*)status_msg, client->sockfd);
            if (IS_NET_ERROR(status))
                return status;
            free(status_msg);
        }

    }

    StatusMsg *status_msg = create_status_msg(STATUS_MSG_SUCCESS, "Login success");
    net_status_t status = send_message((Message*)status_msg, client->sockfd);
    
    if (IS_NET_ERROR(status))
        return status;

    free(status_msg);


    printf("Client %d logged in with username %s\n", client->id, client->name);
    return NET_SUCCESS;
}

static void disconnect_client(Server *server, Client *client)
{
    printf("Client %i disconnected\n", client->id);
    // Removes client
    pthread_mutex_lock(&server->client_list_mutex);
    client_list_remove(server->client_list, client->id);
    pthread_mutex_unlock(&server->client_list_mutex);

}
