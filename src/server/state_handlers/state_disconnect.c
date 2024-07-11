#include "state_handler_utils.h"
#include "broadcast_message.h"

Error* handle_state_disconnect(ServerStateData* handler_data, AppState* _)
{
    Server* server = get_server();
    Client* client = handler_data->client;

    // Tells all the clients that client disconnected
    {
        char text[128];
        sprintf(text, "Used named \"%s\" disconnected!", client->name);
        Message msg = create_server_notification(text);
        send_broadcast_exclude((const Message*)&msg, server, client);
    }

    free_network_connection(&client->status_connection);
    free_network_connection(&client->task_connection);

    // Removes client
    pthread_mutex_lock(&server->client_list_mutex);
    client_list_remove(server->client_list, client->id);
    pthread_mutex_unlock(&server->client_list_mutex);

    // Notifies all clients that a client was removed
    server_client_count_update();

    return CREATE_ERR_OK;
}