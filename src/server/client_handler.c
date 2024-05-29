#include "client_handler.h"
#include "client.h"
#include "state_handler_utils.h"
#include "net/net_stream.h"
#include "net/net_communication.h"
#include "messages/message_types.h"
#include "pthread.h"
#include "application_states.h"

// Used external state handler functions
extern ErrorCode handle_state_connect(ServerStateData*, AppState*);
extern ErrorCode handle_state_login(ServerStateData*, AppState*);
extern ErrorCode handle_state_chat(ServerStateData*, AppState*);
extern ErrorCode handle_state_disconnect(ServerStateData*, AppState*);

/// @brief State machine that handles and executes each state. It also ensures that if any
/// error happens during handling, the client is cleanly disconnected and removed.
static void server_states_handler_fsm(ServerStateData* state_data, AppState initial_state)
{
    AppState curent_state = initial_state;

    // Function pointer of currently used state handler function
    ErrorCode(*handler)(ServerStateData*, AppState*);

    while (true)
    {
        printf("Client %d entered state: ", state_data->client->id);
        // Gets next state handler
        switch (curent_state)
        {
        case APP_STATE_CONNECT:
            handler = handle_state_connect;
            printf("%s\n", "CONNECT");
            break;
        
        case APP_STATE_LOGIN:
            handler = handle_state_login;
            printf("%s\n", "LOGIN");
            break;

        case APP_STATE_CHAT:
            handler = handle_state_chat;
            printf("%s\n", "CHAT");
            break;

        case APP_STATE_DISCONNECT:
            printf("%s\n", "DISCONNECT");

            // Disconnects and exits function
            handle_state_disconnect(state_data, NULL);
            return;

        default:
            printf("Unimplemented state handler type: %i\n", curent_state);
            break;
        }

        // Executes handler
        AppState next_state = APP_STATE_NULL;
        ErrorCode error = handler(state_data, &next_state);

        // Manually sets disconnect state if there is any error
        if (IS_NET_ERROR(error))
        {
            printf("Net error code: %i\n", error);
            curent_state = APP_STATE_DISCONNECT;
        }

        // In case the handler didn't set the next state
        else if (next_state == APP_STATE_NULL)
        {
            printf("Forgot to set next_state in state handler of type %i\n", curent_state);
            exit(EXIT_FAILURE);
        }

        // Usual case where all things were right
        else
            curent_state = next_state;

    }
}


void handle_client_task(ClientHandlerData *handler_data)
{

    // Sets up state data
    ServerStateData state_data;
    state_data.client = handler_data->client;
    state_data.server = handler_data->server;

    // handler_data it's no longer used
    free(handler_data);

    // Handle states
    server_states_handler_fsm(&state_data, APP_STATE_CONNECT);
}



