#include "state_handler_utils.h"
#include "ui/ui.h"

Error* handle_state_quit()
{
    Client* client = get_client();

    ui_set_log_text("Quitting application...");

    // Frees network connection (only if it's connected)
    free_network_connection(&client->net_connection);

    state_handler_set_next(APP_STATE_END);
    return CREATE_ERR_OK;
}
