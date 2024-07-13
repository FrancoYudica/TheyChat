#include <pthread.h>
#include <stdbool.h>
#include "client.h"
#include "they_chat_error.h"
#include "messages/message.h"
#include "server_task_handler.h"
#include "net/net_communication.h"
#include "ui/ui.h"
#include "states_fsm.h"

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

extern Error* server_task_users(TaskStatusPayload);
extern Error* server_task_files(TaskStatusPayload);
extern Error* server_task_upload_file(TaskStatusPayload);
extern Error* server_task_download_file(TaskStatusPayload);
extern Error* server_task_remove_file(TaskStatusPayload);

static void* thread_handler(void*)
{
    set_thread_name(pthread_self(), "Server task handler");
    Message msg;
    Client* client = get_client();
    while (s_running) {

        s_error = wait_for_message_type(
            &client->task_connection,
            &msg,
            MSGT_TASK_STATUS);

        if (!s_running || state_handler_get_current() != APP_STATE_CHAT)
            break;

        if (IS_ERROR(s_error)) {

            if (s_error->code != ERR_NET_PEER_DISCONNECTED)
                ui_push_text_entry(
                    TEXT_ENTRY_TYPE_ERROR,
                    "%s",
                    s_error->message);

            free_error(s_error);
            state_handler_set_next(APP_STATE_DISCONNECT);
            break;
        }

        TaskStatusPayload* task_status = &msg.payload.task_status;

        // Ensures that the received task is executing
        if (task_status->task_status != TASK_STATUS_EXECUTING) {
            ui_push_text_entry(
                TEXT_ENTRY_TYPE_ERROR,
                "Received task of type \"%d\" and it's not executing...",
                task_status->tagged_task.task_type);
            continue;
        }

        // Tells that task will be executed
        ui_set_log_text(
            "Executing task: %d",
            task_status->tagged_task.task_type);

        switch (task_status->tagged_task.task_type) {
        case TASK_USERS:
            s_error = server_task_users(msg.payload.task_status);
            break;
        case TASK_FILES:
            s_error = server_task_files(msg.payload.task_status);
            break;
        case TASK_CLIENT_UPLOAD_FILE:
            s_error = server_task_upload_file(msg.payload.task_status);
            break;

        case TASK_CLIENT_DOWNLOAD_FILE:
            s_error = server_task_download_file(msg.payload.task_status);
            break;

        case TASK_REMOVE_FILE:
            s_error = server_task_remove_file(msg.payload.task_status);
            break;

        default:
            ui_push_text_entry(
                TEXT_ENTRY_TYPE_ERROR,
                "Trying to execute unimplemented task type \"%d\"...",
                task_status->tagged_task.task_type);
            break;
        }

        // If there is any error after executing
        if (IS_ERROR(s_error)) {
            ui_push_text_entry(
                TEXT_ENTRY_TYPE_ERROR,
                "Error during task execution - %s",
                s_error->message);
            ui_set_log_text(
                "Error during %s task execution",
                task_get_name(task_status->tagged_task.task_type));

        } else {
            ui_set_log_text(
                "%s task executed successfully",
                task_get_name(task_status->tagged_task.task_type));
        }
    }
    unregister_thread(pthread_self());

    return NULL;
}
