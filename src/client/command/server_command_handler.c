#include <pthread.h>
#include <stdbool.h>
#include "client.h"
#include "they_chat_error.h"
#include "messages/message.h"
#include "command/server_command_handler.h"
#include "net/net_communication.h"
#include "ui/ui.h"

static pthread_t s_thread;
static Error* s_error;
static bool s_running;

static void* thread_handler(void*);

void server_cmd_handler_init()
{
    pthread_create(&s_thread, NULL, thread_handler, NULL);
    s_running = true;
}
void server_cmd_handler_free()
{
    pthread_detach(s_thread);
    s_running = false;
}

static void* thread_handler(void*)
{
    Message msg;
    Client* client = get_client();
    while (s_running) {
        s_error = wait_for_message_type(
            &client->cmd_connection,
            &msg,
            MSGT_STATUS);

        if (IS_NET_ERROR(s_error)) {
            ui_push_text_entry(
                TEXT_ENTRY_TYPE_WARNING,
                "Error in `server_command_handler thread` while waiting for status message: \"%s\"",
                s_error->message);
        }
    }
    return NULL;
}
