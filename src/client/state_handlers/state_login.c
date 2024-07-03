#include <pthread.h>
#include "state_handler_utils.h"
#include "ui/ui.h"
#include "ui/input_handler.h"

static Error* input_callback(const char* input)
{
    // Sends message telling that the username
    static Message message;
    ClientData* data = (ClientData*)input_handler_get_user_data();
    message = create_user_login_msg(input);
    send_message((const Message*)&message, data->connection_context);

    // Waits confirmation of the login
    Error* err = wait_for_message_type(&data->stream, data->connection_context, &message, MSGT_STATUS);

    if (IS_NET_ERROR(err))
        return err;

    if (message.payload.status.status == STATUS_MSG_FAILURE) {
        ui_push_text_entry(TEXT_ENTRY_TYPE_SERVER, message.payload.status.text);
        ui_set_log_text("A user named \"%s\" already exists. Choose another name", input);
    }

    else {
        strcpy(data->username, input);
        ui_set_log_text("Logged with username \"%s\"", data->username);
        state_handler_set_next(APP_STATE_CHAT);
    }
    return CREATE_ERR_OK;
}

static Error* command_callback(const char* command)
{
    return CREATE_ERR_OK;
}

Error* handle_state_login(ClientData* data)
{
    Message message;
    Error* err;

    // Initializes UI
    ui_init();
    ui_set_connected_count(1);
    ui_set_server_ip(data->connection_details.server_ip);
    ui_set_connected(true);
    ui_set_tls_enabled(data->connection_details.tls_enabled);
    ui_set_input_prompt("Type your username:");

    // Renders the entire UI
    ui_refresh();

    // Sets input callbacks and user data
    input_handler_set_input_callback(NULL);
    input_handler_set_command_callback(NULL);
    input_handler_set_user_data((ClientData*)data);
    input_handler_set_input_callback(input_callback);
    input_handler_set_command_callback(input_callback);

    ui_push_text_entry(TEXT_ENTRY_TYPE_SERVER, "Successfully connected to server!.");
    ui_set_log_text("Enter your unique username");

    // Waits for next state set condition
    state_handler_wait_next_state_cond();

    return CREATE_ERR_OK;
}