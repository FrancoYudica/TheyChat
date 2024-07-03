#include "state_handler_utils.h"

static Error* input_callback(const char* input)
{
    ui_push_text_entry(TEXT_ENTRY_TYPE_LOG, input);
    return CREATE_ERR_OK;
}

static Error* command_callback(const char* input)
{
    return dispatch_command(
        input,
        3,
        CMD_HELP,
        CMD_CONNECT,
        CMD_QUIT);
}

Error* handle_state_offline()
{
    Client* data = get_client();

    ui_set_connected(false);
    ui_set_input_prompt("Type:");
    ui_push_text_entry(TEXT_ENTRY_TYPE_SERVER, "Welcome to They Chat!, you can execute commands now. Use /h to see all commands");

    // Renders the entire UI
    ui_refresh();

    // Sets input callbacks and user data
    input_handler_set_input_callback(NULL);
    input_handler_set_command_callback(NULL);
    input_handler_set_input_callback(input_callback);
    input_handler_set_command_callback(command_callback);

    // Waits for next state set condition
    state_handler_wait_next_state_cond();

    return CREATE_ERR_OK;
}