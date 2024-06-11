#include "server_state_data.h"

ServerStateData create_server_data(Server* server, Client* client)
{
    ServerStateData data;
    data.server = server;
    data.client = client;
    return data;
}
