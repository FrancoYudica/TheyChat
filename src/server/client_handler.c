#include "client_handler.h"
#include "client.h"
#include "state_handler_utils.h"
#include "net/net_stream.h"
#include "net/net_communication.h"
#include "messages/message_types.h"
#include "pthread.h"
#include "application_states.h"

// Used external state handler functions
extern Error* handle_state_connect(ServerStateData*, AppState*);
extern Error* handle_state_login(ServerStateData*, AppState*);
extern Error* handle_state_chat(ServerStateData*, AppState*);
extern Error* handle_state_disconnect(ServerStateData*, AppState*);

/// @brief State machine that handles and executes each state. It also ensures that if any
/// error happens during handling, the client is cleanly disconnected and removed.
static void server_states_handler_fsm(ServerStateData* state_data, AppState initial_state)
{
    AppState current_state = initial_state;

    // Function pointer of currently used state handler function
    Error* (*handler)(ServerStateData*, AppState*);

    while (true) {
        debug_print_client(state_data->client);
        printf("entered state: %s\n", get_application_state_name(current_state));
        // Gets next state handler
        switch (current_state) {
        case APP_STATE_CONNECT:
            handler = handle_state_connect;
            break;

        case APP_STATE_LOGIN:
            handler = handle_state_login;
            break;

        case APP_STATE_CHAT:
            handler = handle_state_chat;
            break;

        case APP_STATE_DISCONNECT:

            // Disconnects and exits function
            handle_state_disconnect(state_data, NULL);
            return;

        default:
            printf("Unimplemented state handler type: %i\n", current_state);
            break;
        }

        state_data->client->current_state = current_state;

        // Executes handler
        AppState next_state = APP_STATE_NULL;
        Error* err = handler(state_data, &next_state);

        // Manually sets disconnect state if there is any error
        if (IS_ERROR(err)) {

            if (err->code != ERR_NET_PEER_DISCONNECTED) {
                printf("Net error code: %i for in client: ", err->code);
                debug_print_client(state_data->client);
                printf("\n");
                print_error(err);
                free_error(err);
            }
            current_state = APP_STATE_DISCONNECT;
        }

        // In case the handler didn't set the next state
        else if (next_state == APP_STATE_NULL) {
            printf("Forgot to set next_state in state handler of type %i\n", current_state);
            exit(EXIT_FAILURE);
        }

        // Usual case where all things were right
        else
            current_state = next_state;
    }
}

void handle_client_task(ClientHandlerData* handler_data)
{

    // Sets up state data
    ServerStateData state_data = create_server_data(handler_data->client);

    // handler_data it's no longer used
    free(handler_data);

    // Handle states
    server_states_handler_fsm(&state_data, APP_STATE_CONNECT);
}
