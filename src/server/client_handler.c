#include "client_handler.h"
#include "client.h"
#include "net/net_stream.h"
#include "net/net_communication.h"

#include "message_types.h"
#include "pthread.h"


// 1- Handle connection
// 2- Handle login
// 3- Handle chat
// 4- Handle disconnect
typedef enum {
    APP_STATE_NULL,
    APP_STATE_CONNECT,
    APP_STATE_LOGIN,
    APP_STATE_CHAT,
    APP_STATE_DISCONNECT
} AppState;

typedef ErrorCode(*ServerHandleState)(ClientHandlerData*, AppState*);

/// @brief Loops until client picks a valid username
static ErrorCode handle_state_connect(ClientHandlerData*, AppState*);

/// @brief Loops until client picks a valid username
static ErrorCode handle_state_login(ClientHandlerData*, AppState*);

/// @brief Loops until client picks a valid username
static ErrorCode handle_state_chat(ClientHandlerData*, AppState*);

/// @brief Removes client from client list
static ErrorCode handle_state_disconnect(ClientHandlerData*, AppState*);


static void handle_states(ClientHandlerData* data, AppState initial_state)
{
    AppState state = initial_state;
    ServerHandleState handler;

    while (true)
    {

        // Gets next state handler
        switch (state)
        {
        case APP_STATE_CONNECT:
            handler = handle_state_connect;
            break;
        
        case APP_STATE_LOGIN:
            handler = handle_state_login;
            break;

        case APP_STATE_CHAT:
            handler = handle_state_chat;
            break;

        case APP_STATE_DISCONNECT:

            // Disconnects and returns
            handle_state_disconnect(data, NULL);
            return;

        default:
            printf("Unimplemented state handler type: %i\n", state);
            break;
        }

        // Executes handler
        AppState next_state = APP_STATE_NULL;
        ErrorCode error = handler(data, &next_state);

        // Manually sets disconnect in case there is any error
        if (IS_NET_ERROR(error))
            state = APP_STATE_DISCONNECT;

        // In case the handler didn't set the next state
        else if (next_state == APP_STATE_NULL)
        {
            printf("Forgot to set next_state in state handler of type %i\n", state);
            exit(EXIT_FAILURE);
        }

        // Usual case where all things were right
        else
            state = next_state;

    }
}


void handle_client_task(ClientHandlerData *handler_data)
{
    handle_states(handler_data, APP_STATE_CONNECT);
}


static ErrorCode handle_state_connect(ClientHandlerData *handler_data, AppState *next_state)
{
    // Tells client that it's connected
    Bytes128Msg *connected_message = create_client_connected();
    ErrorCode status = send_message((const Message*)connected_message, handler_data->client->sockfd);
    free(connected_message);
    *next_state = APP_STATE_LOGIN;
    return status;
}

static ErrorCode handle_state_login(ClientHandlerData *handler_data, AppState *next_state)
{
    Client *client = handler_data->client;
    while(true)
    {
        UserLoginMsg *user_login_msg;

        // Waits for login
        ErrorCode status = wait_for_message_type(&client->stream, client->sockfd, (Message**)&user_login_msg, MSGT_USER_LOGIN);

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
            ErrorCode status = send_message((Message*)status_msg, client->sockfd);
            if (IS_NET_ERROR(status))
                return status;
            free(status_msg);
        }

    }

    StatusMsg *status_msg = create_status_msg(STATUS_MSG_SUCCESS, "Login success");
    ErrorCode status = send_message((Message*)status_msg, client->sockfd);
    
    if (IS_NET_ERROR(status))
        return status;

    free(status_msg);

    printf("Client %d logged in with username %s\n", client->id, client->name);
    *next_state = APP_STATE_CHAT;
    return ERR_NET_OK;
}

static ErrorCode handle_state_chat(ClientHandlerData *handler_data, AppState *next_state)
{
    Client *client = handler_data->client;
    UserChatMsg *msg = create_user_chat_msg("Hey!", "SERVER");

    ErrorCode error = ERR_NET_OK;

    while(true)
    {
        ErrorCode send_status = send_message((const Message*)msg, client->sockfd);
        
        if (IS_NET_ERROR(send_status))
        {
            if (send_status != ERR_PEER_DISCONNECTED)
                printf("NETWORK ERROR type %i while handling client %d\n", send_status, client->id);

            error = send_status;
            break;
        }

        // Waits for any message
        Message *client_msg;
        ErrorCode receive_status = wait_for_message(&client->stream, client->sockfd, &client_msg);

        if (IS_NET_ERROR(receive_status))
        {
            if (receive_status != ERR_PEER_DISCONNECTED)
                printf("NETWORK ERROR type %i while handling client %d\n", receive_status, client->id);

            error = receive_status;
            break;
        }
        // Outputs client message
        print_message(client_msg);
        free(client_msg);
    }

    free(msg);
    *next_state = APP_STATE_DISCONNECT;
    return error;
}

static ErrorCode handle_state_disconnect(ClientHandlerData *handler_data, AppState *next_state)
{
    Client *client = handler_data->client;
    Server *server = handler_data->server;
    
    printf("Client %i disconnected\n", client->id);
    // Removes client
    pthread_mutex_lock(&server->client_list_mutex);
    client_list_remove(server->client_list, client->id);
    pthread_mutex_unlock(&server->client_list_mutex);

    return ERR_NET_OK;
}
