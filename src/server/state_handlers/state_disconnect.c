#include "state_handler_utils.h"
#include "broadcast_message.h"

Error* handle_state_disconnect(ServerStateData* handler_data, AppState* _)
{
    Message message;
    Server* server = get_server();
    Client* client = handler_data->client;

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

    send_broadcast_exclude(&message, server, client);

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