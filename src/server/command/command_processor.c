#include "command_types.h"
#include "command/command_processor.h"
#include "net/net_communication.h"

Error* process_users_command(ServerStateData* data)
{
    Error* err;
    Message message;

    // Sequence start
    message = create_seq_start_msg();
    err = send_message((const Message*)&message, data->client->connection_context);
    if (IS_NET_ERROR(err))
        return err;

    Client* client;
    ClientList* clients = data->server->client_list;

    pthread_mutex_lock(&data->server->broadcast_mutex);
    client_list_interator_rewind(clients);

    // Iterates through all the clients and sends the client name
    while ((client = client_list_interator_next(clients)) != NULL) {
        message = create_heap_seq_msg(client->name, strlen(client->name) + 1);
        printf("Name: %s\n", client->name);
        err = send_message((const Message*)&message, data->client->connection_context);

        free(message.payload.heap_sequence.payload);

        if (IS_NET_ERROR(err))
            return err;
    }

    pthread_mutex_unlock(&data->server->broadcast_mutex);

    // Sequence end
    message = create_seq_end_msg();
    err = send_message(&message, data->client->connection_context);
    return err;
}

Error* execute_command_processor(ServerStateData* data, Message* command_message)
{
    printf("RECEIVED COMMAND MESSAGE!\n");

    switch (command_message->payload.command.command_type) {
    case CMD_DISCONNECT:
        return CREATE_ERR(ERR_NET_PEER_DISCONNECTED, "Peer disconnected");

    case CMD_USERS:
        return process_users_command(data);

    default:
        break;
    }

    return CREATE_ERR_OK;
}