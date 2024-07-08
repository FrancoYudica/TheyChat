#include <pthread.h>
#include <stdbool.h>
#include "client.h"
#include "they_chat_error.h"
#include "messages/message.h"
#include "server_task/server_task_handler.h"
#include "net/net_communication.h"
#include "ui/ui.h"

static pthread_t s_thread;
static Error* s_error;
static bool s_running;

static void* thread_handler(void*);

void server_task_handler_init()
{
    pthread_create(&s_thread, NULL, thread_handler, NULL);
    s_running = true;
}
void server_task_handler_free()
{
    pthread_detach(s_thread);
    s_running = false;
}

static void* thread_handler(void*)
{
    set_thread_name(pthread_self(), "Server task handler");
    Message msg;
    Client* client = get_client();
    while (s_running) {
        s_error = wait_for_message_type(
            &client->task_connection,
            &msg,
            MSGT_STATUS);

        if (IS_NET_ERROR(s_error)) {

            if (s_error->code != ERR_NET_PEER_DISCONNECTED)
                ui_push_text_entry(
                    TEXT_ENTRY_TYPE_WARNING,
                    "%s",
                    s_error->message);

            free_error(s_error);
        }
        {
            ui_push_text_entry(
                TEXT_ENTRY_TYPE_SERVER,
                "%s",
                msg.payload.status.text);
        }
    }
    unregister_thread(pthread_self());

    return NULL;
}
