#include "state_handler_utils.h"

ErrorCode handle_state_connect(ServerStateData* handler_data, AppState* next_state)
{
    // Tells client that it's connected
    Message message = create_client_connected();
    ErrorCode err = send_message(
        (const Message*)&message,
        handler_data->client->connection_context);

    if (IS_NET_ERROR(err))
        return err;

    *next_state = APP_STATE_LOGIN;

    return ERR_OK;
}