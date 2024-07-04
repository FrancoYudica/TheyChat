#ifndef __CLIENT_STATES_FSM_H__
#define __CLIENT_STATES_FSM_H__
#include "application_states.h"
#include "client.h"

/// @brief State machine that handles and executes each state. It also ensures that if any
/// error happens in any state, the application exits safely.
void state_handler_fsm(AppState initial_state);

void state_handler_set_next(AppState next_state);

/// @brief Blocks thread and waits for next state set condition
void state_handler_wait_next_state_cond();

/// @brief Returns current application state
AppState state_handler_get_current();

#endif