#include "broadcast_message.h"
#include "net/net_communication.h"

void send_broadcast(const Message* msg, Server* server)
{
    Client* client;
    ClientList* clients = server->client_list;
    client_list_interator_rewind(clients);

    pthread_mutex_lock(&server->broadcast_mutex);

    // Iterates through all the clients
    while ((client = client_list_interator_next(clients)) != NULL) {

        if (client->current_state == APP_STATE_CHAT)
            send_message(msg, &client->status_connection);
    }

    pthread_mutex_unlock(&server->broadcast_mutex);
}

void send_broadcast_exclude(const Message* msg, Server* server, Client* exclude_client)
{
    Client* client;
    ClientList* clients = server->client_list;
    client_list_interator_rewind(clients);

    pthread_mutex_lock(&server->broadcast_mutex);

    // Iterates through all the clients
    while ((client = client_list_interator_next(clients)) != NULL) {

        if (client != exclude_client && client->current_state == APP_STATE_CHAT)
            send_message(msg, &client->status_connection);
    }

    pthread_mutex_unlock(&server->broadcast_mutex);
}
