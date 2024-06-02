#include "state_handler_utils.h"

ErrorCode handle_state_connect(ServerStateData* handler_data, AppState* next_state)
{
    Client* client = handler_data->client;
    // Tells client that it's connected
    Bytes128Msg* connected_message = create_client_connected();
    ErrorCode err = send_message((const Message*)connected_message, client->sockfd);
    free(connected_message);

    if (IS_NET_ERROR(err))
        return err;

    // Waits for client response
    StatusMsg* status_msg;
    err = wait_for_message_type(&client->stream, client->sockfd, (Message**)&status_msg, MSGT_STATUS);

    if (IS_NET_ERROR(err))
        return err;

    bool status = status_msg->status;

    free(status_msg);

    if (!status)
        return ERR_PEER_DISCONNECTED;

    *next_state = APP_STATE_LOGIN;

    return ERR_NET_OK;
}