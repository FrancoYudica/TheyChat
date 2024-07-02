#include <pthread.h>
#include "state_handler_utils.h"
#include "ui/ui.h"
#include "ui/input_handler.h"

static pthread_mutex_t s_mutex;
static pthread_cond_t s_logged_cond;

static void push_entry(const char* text)
{
    ChatEntry entry;
    entry.type = CHAT_ENTRY_SERVER_NOTIFICATION;
    strcpy(entry.data.server_notification.text, text);
    chat_entry_format_time(&entry, time(NULL));
    ui_add_chat_entry(entry);
}

static Error* input_callback(const char* input)
{
    // Sends message telling that the username
    static Message message;
    ClientData* data = (ClientData*)input_handler_get_user_data();
    message = create_user_login_msg(input);
    push_entry(input);
    send_message((const Message*)&message, data->connection_context);

    // Waits confirmation of the login
    Error* err = wait_for_message_type(&data->stream, data->connection_context, &message, MSGT_STATUS);

    if (IS_NET_ERROR(err))
        return err;

    if (message.payload.status.status == STATUS_MSG_FAILURE)
        push_entry(message.payload.status.text);
    else {
        strcpy(data->username, input);
        pthread_cond_signal(&s_logged_cond);
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

    pthread_mutex_init(&s_mutex, NULL);
    pthread_cond_init(&s_logged_cond, NULL);

    // Initializes UI
    ui_init();
    ui_set_connected_count(1);
    ui_set_server_ip(data->connection_details.server_ip);
    ui_set_connected(true);
    ui_set_tls_enabled(data->connection_details.tls_enabled);

    // Renders the entire UI
    ui_refresh();

    // Sets input callbacks and user data
    input_handler_set_input_callback(NULL);
    input_handler_set_command_callback(NULL);
    input_handler_set_user_data((ClientData*)data);
    input_handler_set_input_callback(input_callback);
    input_handler_set_command_callback(input_callback);

    push_entry("Successfully connected to server!. Please enter your unique username");

    // Waits for login condition
    pthread_cond_wait(&s_logged_cond, &s_mutex);

    pthread_mutex_destroy(&s_mutex);
    pthread_cond_destroy(&s_logged_cond);

    state_handler_set_next(APP_STATE_CHAT);
    return CREATE_ERR_OK;
}