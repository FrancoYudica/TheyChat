#include "states_fsm.h"
#include "ui/ui.h"

Error* disconnect_handler(uint8_t, char**)
{
    ui_set_log_text("/disconnect command executed");
    state_handler_set_next(APP_STATE_DISCONNECT);
    return CREATE_ERR_OK;
}