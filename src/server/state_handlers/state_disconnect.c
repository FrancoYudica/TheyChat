#include "state_handler_utils.h"
#include "broadcast_message.h"

Error* handle_state_disconnect(ServerStateData* handler_data, AppState* _)
{
    Client* client = handler_data->client;
    Server* server = handler_data->server;

    // Tells all the clients that client disconnected
    {
        char text[128];
        sprintf(text, "Used named \"%s\" disconnected!", client->name);
        Message msg = create_server_notification(text);
        send_broadcast_exclude((const Message*)&msg, server, client);
    }

    // Removes client
    pthread_mutex_lock(&server->client_list_mutex);
    client_list_remove(server->client_list, client->id);
    pthread_mutex_unlock(&server->client_list_mutex);

    // Notifies all clients that a client was removed
    server_client_count_update(server);

    return CREATE_ERR_OK;
}