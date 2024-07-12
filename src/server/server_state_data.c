#include "server_state_data.h"

ServerStateData create_server_data(uint32_t client_id)
{
    ServerStateData data;
    data.client_id = client_id;
    return data;
}
