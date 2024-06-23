#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "messages/message.h"
#include "application_states.h"
#include "messages/message_types.h"
#include "net/serialization/net_message_serializer.h"
#include "net/net_communication.h"
#include "client_data.h"
#include "states_fsm.h"

int main(int argc, char** argv)
{
    ClientData client;

#ifdef THEY_CHAT_SSL
    client.connection_details.tls_enabled = true;
#else
    client.connection_details.tls_enabled = false;
#endif

    net_init();

    // Initializes network stream
    init_net_stream(&client.stream);

    // Starts execution of FSM
    printf("Entering application...\n");
    client_states_handler_fsm(&client, APP_STATE_OFFLINE);

    net_shutdown();
    return EXIT_SUCCESS;
}
