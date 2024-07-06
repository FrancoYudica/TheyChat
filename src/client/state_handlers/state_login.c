#include "state_handler_utils.h"

static Error* input_callback(const char* input)
{
    // Sends message telling that the username
    static Message message;

    Client* client = get_client();
    message = create_user_login_msg(input);
    send_message((const Message*)&message, &client->status_connection);

    // Waits confirmation of the login
    Error* err = wait_for_message_type(&client->status_connection, &message, MSGT_STATUS);

    if (IS_NET_ERROR(err))
        return err;

    if (message.payload.status.status == STATUS_MSG_FAILURE) {
        ui_push_text_entry(TEXT_ENTRY_TYPE_SERVER, message.payload.status.text);
        ui_set_log_text("A user named \"%s\" already exists. Choose another name", input);
    }

    else {
        strcpy(client->username, input);
        ui_set_log_text("Logged with username \"%s\"", client->username);
        state_handler_set_next(APP_STATE_CHAT);
    }
    return CREATE_ERR_OK;
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

Error* handle_state_login()
{
    Client* client = get_client();
    Message message;
    Error* err;

    ui_set_input_prompt("Type your username:");

    // Renders the entire UI
    ui_refresh();

    // Sets input callbacks and user client
    input_handler_set_input_callback(NULL);
    input_handler_set_command_callback(NULL);
    input_handler_set_input_callback(input_callback);
    input_handler_set_command_callback(command_callback);

    ui_push_text_entry(TEXT_ENTRY_TYPE_SERVER, "Successfully connected to server!.");
    ui_set_log_text("Enter your unique username");

    // Waits for next state set condition
    state_handler_wait_next_state_cond();

    return CREATE_ERR_OK;
}