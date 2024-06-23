#include "application_states.h"

const char* get_application_state_name(AppState state)
{
    static const char* state_names[] = {
        [APP_STATE_NULL] = "STATE_NULL",
        [APP_STATE_ERROR] = "STATE_ERROR",
        [APP_STATE_OFFLINE] = "STATE_OFFLINE",
        [APP_STATE_CONNECT] = "STATE_CONNECT",
        [APP_STATE_QUEUE] = "STATE_QUEUE",
        [APP_STATE_LOGIN] = "STATE_LOGIN",
        [APP_STATE_CHAT] = "STATE_CHAT",
        [APP_STATE_DISCONNECT] = "STATE_DISCONNECT"
    };

    return state_names[state];
}
