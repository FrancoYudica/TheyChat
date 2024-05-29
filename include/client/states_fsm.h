#ifndef __CLIENT_STATES_FSM_H__
#define __CLIENT_STATES_FSM_H__
#include "common/application_states.h"
#include "client/client_data.h"

/// @brief State machine that handles and executes each state. It also ensures that if any
/// error happens in any state, the application exits safely.
void client_states_handler_fsm(ClientData* data, AppState initial_state);

#endif