#include "server.h"
#include "messages/message_types.h"
#include "broadcast_message.h"

void server_client_count_update(Server* server)
{
    // Sends to all clients the currently connected clients
    uint32_t connected_client_count = client_list_length(server->client_list);
    ConnectedClientsMsg* connected_clients = create_connected_clients_msg(connected_client_count);
    send_broadcast((const Message*)connected_clients, server);
    free(connected_clients);
}
