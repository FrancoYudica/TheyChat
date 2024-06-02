#include "state_handler_utils.h"
#include "broadcast_message.h"

ErrorCode handle_state_disconnect(ServerStateData* handler_data, AppState* _)
{
    Client* client = handler_data->client;
    Server* server = handler_data->server;

    // Tells all the clients that client disconnected
    {
        char text[128];
        sprintf(text, "Used named \"%s\" disconnected!", client->name);
        UserChatMsg* msg = create_user_chat_msg(text, "SERVER");
        send_broadcast_exclude((const Message*)msg, handler_data, client);
        free(msg);
    }

    // Removes client
    pthread_mutex_lock(&server->client_list_mutex);
    client_list_remove(server->client_list, client->id);
    pthread_mutex_unlock(&server->client_list_mutex);

    return ERR_NET_OK;
}