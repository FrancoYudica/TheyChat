#include "task/task_handler_data.h"
#include "net/net_communication.h"

Error* server_task_users(TaskHandlerData* data)
{
    Error* err;
    Message message;

    // Sequence start
    message = create_seq_start_msg();
    err = send_message((const Message*)&message, &data->client->task_connection);
    if (IS_NET_ERROR(err))
        return err;

    Client* client;
    ClientList* clients = data->server->client_list;

    pthread_mutex_lock(&data->server->broadcast_mutex);
    client_list_interator_rewind(clients);

    TaskUsersDada* users_data = &data->task_request.tagged_task.data.users;

    char buffer[512];
    uint32_t space = 10;

    // Iterates through all the clients and sends the client name
    while ((client = client_list_interator_next(clients)) != NULL) {

        // Formats string
        sprintf(buffer, "%-*s", space, client->name);
        if (users_data->show_ip)

            sprintf(
                buffer + strlen(buffer),
                " %s",
                client->ip);

        if (users_data->show_id)
            sprintf(
                buffer + strlen(buffer),
                " %d",
                client->id);

        // Creates message and sends
        message = create_heap_seq_msg(buffer, strlen(buffer) + 1);
        err = send_message((const Message*)&message, &data->client->task_connection);

        // Frees heap memory
        free(message.payload.heap_sequence.payload);

        if (IS_NET_ERROR(err))
            return err;
    }

    pthread_mutex_unlock(&data->server->broadcast_mutex);

    // Sequence end
    message = create_seq_end_msg();
    err = send_message(&message, &data->client->task_connection);
    return err;
}
