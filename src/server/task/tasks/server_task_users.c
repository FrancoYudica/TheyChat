#include "task/task_handler_data.h"
#include "net/net_communication.h"
#include "string/string_list.h"
#include "utils/string_list_transferring.h"

Error* server_task_users(TaskHandlerData* data)
{
    Error* err;
    Server* server = get_server();
    Client* client = client_list_find_by_id(server->client_list, data->client_id);
    ClientList* clients = server->client_list;

    pthread_mutex_lock(&server->client_list_mutex);
    client_list_interator_rewind(clients);

    TaskUsersDada* users_data = &data->task_request.tagged_task.data.users;

    char buffer[512];
    uint32_t space = 10;
    StringList* str_list = string_list_create();

    // Iterates through all clients and fills string list
    Client* other_client;
    while ((other_client = client_list_interator_next(clients))) {

        // Formats string
        sprintf(buffer, "%-*s", space, other_client->name);
        if (users_data->show_ip)

            sprintf(
                buffer + strlen(buffer),
                "%-*s",
                (int)(MAX_IP_BYTES + 2),
                other_client->ip);

        if (users_data->show_id)
            sprintf(
                buffer + strlen(buffer),
                "%d",
                other_client->id);

        string_list_add(str_list, buffer);
    }

    pthread_mutex_unlock(&server->client_list_mutex);

    // Sends string list to client
    err = send_string_list(&client->task_connection, str_list);
    string_list_destroy(str_list);
    return err;
}
