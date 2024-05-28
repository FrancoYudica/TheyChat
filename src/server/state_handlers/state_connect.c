#include "server/state_handler_utils.h"


ErrorCode handle_state_connect(ServerStateData *handler_data, AppState *next_state)
{
    // Tells client that it's connected
    Bytes128Msg *connected_message = create_client_connected();
    ErrorCode status = send_message((const Message*)connected_message, handler_data->client->sockfd);
    free(connected_message);
    *next_state = APP_STATE_LOGIN;
    return status;
}