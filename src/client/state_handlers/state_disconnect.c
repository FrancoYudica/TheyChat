#include "state_handler_utils.h"

extern Error* handle_state_disconnect()
{
    Client* client = get_client();
    free_network_connection(&client->status_connection);
    free_network_connection(&client->cmd_connection);
    state_handler_set_next(APP_STATE_OFFLINE);
    return CREATE_ERR_OK;
}
