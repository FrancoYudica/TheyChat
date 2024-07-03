#include "state_handler_utils.h"

extern Error* handle_state_disconnect()
{
    Client* data = get_client();
    net_close(data->connection_context);
    state_handler_set_next(APP_STATE_OFFLINE);
    return CREATE_ERR_OK;
}
