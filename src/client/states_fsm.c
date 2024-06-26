#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "states_fsm.h"
#include "they_chat_error.h"

extern Error* handle_state_offline(ClientData* data, AppState* next_state);
extern Error* handle_state_connect(ClientData* data, AppState* next_state);
extern Error* handle_state_queue(ClientData* data, AppState* next_state);
extern Error* handle_state_login(ClientData* data, AppState* next_state);
extern Error* handle_state_chat(ClientData* data, AppState* next_state);
extern Error* handle_state_disconnect(ClientData* data, AppState* next_state);

void client_states_handler_fsm(ClientData* data, AppState initial_state)
{
    AppState curent_state = initial_state;

    // Function pointer of currently used state handler function
    Error* (*handler)(ClientData*, AppState*);

    while (true) {
        // Gets next state handler
        switch (curent_state) {
        case APP_STATE_OFFLINE:
            handler = handle_state_offline;
            break;

        case APP_STATE_CONNECT:
            handler = handle_state_connect;
            break;

        case APP_STATE_ONQUEUE:
            handler = handle_state_queue;
            break;

        case APP_STATE_LOGIN:
            handler = handle_state_login;
            break;

        case APP_STATE_CHAT:
            handler = handle_state_chat;
            break;

        case APP_STATE_DISCONNECT:
            handler = handle_state_disconnect;
            break;

        default:
            printf("Unimplemented state handler type: %i\n", curent_state);
            break;
        }

        // Executes handler
        AppState next_state = APP_STATE_NULL;
        Error* err = handler(data, &next_state);

        // Manually sets disconnect state if there is any error
        if (IS_NET_ERROR(err)) {
            print_error(err);
            curent_state = APP_STATE_DISCONNECT;
            free_error(err);
        }

        // In case the handler didn't set the next state
        else if (next_state == APP_STATE_NULL) {
            printf("Forgot to set next_state in state handler of type %i\n", curent_state);
            exit(EXIT_FAILURE);
        }

        // Usual case where all things were right
        else
            curent_state = next_state;
    }
}