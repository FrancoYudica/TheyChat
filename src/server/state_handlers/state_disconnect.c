#include "server/state_handler_utils.h"

ErrorCode handle_state_disconnect(ServerStateData *handler_data, AppState *_)
{
    Client *client = handler_data->client;
    Server *server = handler_data->server;
    
    // Removes client
    pthread_mutex_lock(&server->client_list_mutex);
    client_list_remove(server->client_list, client->id);
    pthread_mutex_unlock(&server->client_list_mutex);
    
    return ERR_NET_OK;
}