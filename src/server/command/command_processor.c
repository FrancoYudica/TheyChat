#include "command/command_processor.h"
#include "net/net_communication.h"

ErrorCode process_users_command(ServerStateData* data)
{
    ErrorCode err;

    // Sequence start
    SequenceStartMsg* start = create_seq_start_msg();
    err = send_message((const Message*)start, data->client->connection_context);
    free(start);
    if (IS_NET_ERROR(err))
        return err;

    Client* client;
    ClientList* clients = data->server->client_list;

    pthread_mutex_lock(&data->server->broadcast_mutex);
    client_list_interator_rewind(clients);

    // Iterates through all the clients and sends the client name
    while ((client = client_list_interator_next(clients)) != NULL) {
        HeapSequenceMsg* client_seq = create_heap_seq_msg(client->name, strlen(client->name) + 1);
        printf("Name: %s\n", client->name);
        err = send_message((const Message*)client_seq, data->client->connection_context);

        free(client_seq->payload);
        free(client_seq);

        if (IS_NET_ERROR(err))
            return err;
    }

    pthread_mutex_unlock(&data->server->broadcast_mutex);

    // Sequence end
    SequenceEndMsg* end = create_seq_end_msg();
    err = send_message(end, data->client->connection_context);
    free(end);
    return err;
}

ErrorCode execute_command_processor(ServerStateData* data, CommandMsg* command_message)
{
    printf("RECEIVED COMMAND MESSAGE!\n");

    switch (command_message->command_type) {
    case CMDT_DISCONNECT:
        return ERR_PEER_DISCONNECTED;
        break;

    case CMDT_USERS: {
        return process_users_command(data);
        break;
    }

    default:
        break;
    }

    return ERR_NET_OK;
}