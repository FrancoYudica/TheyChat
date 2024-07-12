#include "state_handler_utils.h"

Error* handle_state_connect(
    ServerStateData* state_data,
    AppState* next_state)
{
    Client* client = client_list_find_by_id(get_server()->client_list, state_data->client_id);

    // Client disconnected
    if (client == NULL) {
        *next_state = APP_STATE_END;
        printf("A client disconnected during queue\n");
        return CREATE_ERR_OK;
    }

    // Tells client that it's connected
    Message message = create_client_connected();
    Error* err = send_message(
        (const Message*)&message,
        &client->status_connection);

    if (IS_ERROR(err))
        return err;

    *next_state = APP_STATE_LOGIN;

    return CREATE_ERR_OK;
}