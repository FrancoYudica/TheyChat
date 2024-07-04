#include "state_handler_utils.h"

static pthread_t s_receive_thread;

static void* wait_for_accept()
{
    Client* data = get_client();
    Message message;
    Error* err;
    ui_set_log_text("You are on queue, please wait...");

    // Waits for connection
    bool waiting_in_queue = true;
    while (waiting_in_queue) {
        err = wait_for_message(&data->stream, data->connection_context, &message);

        if (IS_NET_ERROR(err))
            return NULL;

        // Client connected successfully;
        if (message.type == MSGT_CLIENT_CONNECTED)
            waiting_in_queue = false;

        // Client in queue
        else if (message.type == MSGT_CLIENT_ON_QUEUE) {
            ui_set_log_text("%s", message.payload.bytes_128.bytes);
        } else {
            ui_set_log_text("Received unexpected file type %i", message.type);
        }
    }

    state_handler_set_next(APP_STATE_LOGIN);
    return NULL;
}

static Error* command_callback(const char* command)
{
    return dispatch_command(
        command,
        3,
        CMD_HELP,
        CMD_DISCONNECT,
        CMD_QUIT);
}

Error* handle_state_queue()
{
    Client* data = get_client();
    Message message;
    Error* err;

    ui_set_input_prompt("Type:");

    // Renders the entire UI
    ui_refresh();

    // Sets input callbacks and user data
    input_handler_set_input_callback(NULL);
    input_handler_set_command_callback(NULL);
    input_handler_set_command_callback(command_callback);

    pthread_create(&s_receive_thread, NULL, wait_for_accept, NULL);

    // Waits for next state set condition
    state_handler_wait_next_state_cond();

    pthread_detach(s_receive_thread);

    return CREATE_ERR_OK;
}