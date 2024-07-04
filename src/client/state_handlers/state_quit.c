#include "state_handler_utils.h"
#include "ui/ui.h"

Error* handle_state_quit()
{
    Client* client = get_client();

    ui_set_log_text("Quitting application...");

    if (client->connection_context != NULL) {
        net_close(client->connection_context);
        client->connection_context = NULL;
    }

    state_handler_set_next(APP_STATE_END);
    return CREATE_ERR_OK;
}
