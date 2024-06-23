#include <unistd.h>
#include "state_handler_utils.h"

extern Error* handle_state_disconnect(ClientData* data, AppState* next_state)
{
    net_close(data->connection_context);
    *next_state = APP_STATE_OFFLINE;
    return CREATE_ERR_OK;
}
