#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include "states_fsm.h"
#include "they_chat_error.h"

extern Error* handle_state_offline(ClientData*);
extern Error* handle_state_connect(ClientData*);
extern Error* handle_state_queue(ClientData*);
extern Error* handle_state_login(ClientData*);
extern Error* handle_state_chat(ClientData*);
extern Error* handle_state_disconnect(ClientData*);

static AppState s_current_state = APP_STATE_NULL;
static pthread_mutex_t s_cond_mutex;
static pthread_cond_t s_next_state_cond;

void state_handler_fsm(ClientData* data, AppState initial_state)
{
    pthread_mutex_init(&s_cond_mutex, NULL);
    pthread_cond_init(&s_next_state_cond, NULL);

    s_current_state = initial_state;

    // Function pointer of currently used state handler function
    Error* (*handler)(ClientData*);

    while (true) {
        // Gets next state handler
        switch (s_current_state) {
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
            printf("Unimplemented state handler type: %i\n", s_current_state);
            break;
        }

        // Executes handler
        AppState executing_state = s_current_state;
        s_current_state = APP_STATE_NULL;
        Error* err = handler(data);

        // Manually sets disconnect state if there is any error
        if (IS_NET_ERROR(err)) {
            print_error(err);
            s_current_state = APP_STATE_DISCONNECT;
            free_error(err);
        }

        // In case the handler didn't set the next state
        else if (s_current_state == APP_STATE_NULL) {
            printf("Forgot to set next_state in state handler of type %i\n", executing_state);
            exit(EXIT_FAILURE);
        }
    }
}

void state_handler_set_next(AppState next_state)
{
    // Ensures that setting the state doesn't override any
    // other state previously set. In a good architecture,
    // this shouldn't fail, since next states aren't overwritten
    assert(s_current_state == APP_STATE_NULL || next_state == APP_STATE_NULL);
    s_current_state = next_state;
    pthread_cond_signal(&s_next_state_cond);
}

void state_handler_wait_next_state_cond()
{
    pthread_cond_wait(&s_next_state_cond, &s_cond_mutex);
}
