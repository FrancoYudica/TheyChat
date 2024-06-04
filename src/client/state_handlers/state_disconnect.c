#include <unistd.h>
#include "state_handler_utils.h"

extern ErrorCode handle_state_disconnect(ClientData* data, AppState* _)
{
    net_close(&data->connection_context);
    return ERR_NET_OK;
}
