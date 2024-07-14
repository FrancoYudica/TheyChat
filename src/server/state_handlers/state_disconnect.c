#include "state_handler_utils.h"
#include "broadcast_message.h"

Error* handle_state_disconnect(
    ServerStateData* state_data,
    AppState* app_state)
{
    Message message;
    Server* server = get_server();
    pthread_mutex_lock(&server->client_list_mutex);
    Client* client = client_list_find_by_id(server->client_list, state_data->client_id);

    *app_state = APP_STATE_END;

    if (client == NULL) {
        pthread_mutex_unlock(&server->client_list_mutex);
        return CREATE_ERR_OK;
    }

    // Removes all client files
    uint32_t removed_count = 0;
    server_remove_client_files(client->id, &removed_count);

    if (removed_count == 0)
        message = create_server_notification(
            "\"%s\" disconnected!",
            client->name);
    else
        message = create_server_notification(
            "\"%s\" disconnected!. It's %d files where removed",
            client->name,
            removed_count);

    send_broadcast_exclude(&message, client);

    free_network_connection(&client->status_connection);
    free_network_connection(&client->task_connection);

    // Removes client
    client_list_remove(server->client_list, client->id);
    pthread_mutex_unlock(&server->client_list_mutex);

    // Notifies all clients that a client was removed
    server_client_count_update();

    return CREATE_ERR_OK;
}