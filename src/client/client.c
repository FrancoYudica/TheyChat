#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "messages/message.h"
#include "application_states.h"
#include "messages/message_types.h"
#include "net/serialization/net_message_serializer.h"
#include "net/net_communication.h"
#include "client.h"
#include "states_fsm.h"
#include "ui/ui.h"

int main()
{
    Client* client = get_client();
    init_network_connection(&client->status_connection);

#ifdef THEY_CHAT_SSL
    client->connection_details.tls_enabled = true;
#else
    client->connection_details.tls_enabled = false;
#endif

    net_init();

    // Initializes UI
    ui_init();
    ui_set_connected(false);

    // Starts execution of FSM
    state_handler_fsm(APP_STATE_OFFLINE);

    ui_free();
    net_shutdown();
    return EXIT_SUCCESS;
}
