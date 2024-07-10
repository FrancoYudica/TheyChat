#include "task/task_handler_data.h"
#include "net/net_communication.h"
#include "string/string_list.h"
#include "utils/string_list_transferring.h"

Error* server_task_users(TaskHandlerData* data)
{
    Error* err;

    ClientList* clients = data->server->client_list;

    pthread_mutex_lock(&data->server->client_list_mutex);
    client_list_interator_rewind(clients);

    TaskUsersDada* users_data = &data->task_request.tagged_task.data.users;

    char buffer[512];
    uint32_t space = 10;
    StringList* str_list = string_list_create();

    // Iterates through all clients and fills string list
    Client* client;
    while ((client = client_list_interator_next(clients))) {

        // Formats string
        sprintf(buffer, "%-*s", space, client->name);
        if (users_data->show_ip)

            sprintf(
                buffer + strlen(buffer),
                "%-*s",
                (int)(MAX_IP_BYTES + 2),
                client->ip);

        if (users_data->show_id)
            sprintf(
                buffer + strlen(buffer),
                "%d",
                client->id);

        string_list_add(str_list, buffer);
    }

    pthread_mutex_unlock(&data->server->client_list_mutex);

    // Sends string list to client
    err = send_string_list(&data->client->task_connection, str_list);
    string_list_destroy(str_list);
    return err;
}
