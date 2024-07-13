#include "states_fsm.h"
#include "ui/ui.h"

Error* quit_handler(uint8_t, char**)
{
    ui_set_log_text("Quitting application");
    state_handler_set_next(APP_STATE_QUIT);
    return CREATE_ERR_OK;
}