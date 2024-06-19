#include "state_handler_utils.h"

ErrorCode handle_state_connect(ServerStateData* handler_data, AppState* next_state)
{
    Client* client = handler_data->client;
    Message message;
    ErrorCode err;
    {
        // Tells client that it's connected
        message = create_client_connected();
        err = send_message((const Message*)&message, client->connection_context);

        if (IS_NET_ERROR(err))
            return err;
    }

    // Waits for client response
    err = wait_for_message_type(&client->stream, client->connection_context, (Message**)&message, MSGT_STATUS);

    if (IS_NET_ERROR(err))
        return err;

    bool status = message.payload.status.status;

    if (!status)
        return ERR_PEER_DISCONNECTED;

    *next_state = APP_STATE_LOGIN;

    return ERR_NET_OK;
}