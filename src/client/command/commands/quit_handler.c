#include "states_fsm.h"

Error* quit_handler(uint8_t, char**)
{
    state_handler_set_next(APP_STATE_QUIT);
    return CREATE_ERR_OK;
}