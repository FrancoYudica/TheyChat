#include "states_fsm.h"

Error* disconnect_handler(uint8_t, char**)
{
    state_handler_set_next(APP_STATE_DISCONNECT);
    return CREATE_ERR_OK;
}