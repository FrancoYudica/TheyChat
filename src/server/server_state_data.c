#include "server_state_data.h"

ServerStateData create_server_data(Client* client)
{
    ServerStateData data;
    data.client = client;
    return data;
}
