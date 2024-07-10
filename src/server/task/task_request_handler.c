#include "server.h"
#include "task/task_handler_data.h"
#include "net/net_communication.h"
#include "server_state_data.h"

extern Error* server_task_users(TaskHandlerData*);
extern Error* server_task_files(TaskHandlerData*);
extern Error* server_task_upload_file(TaskHandlerData*);
extern Error* server_task_download_file(TaskHandlerData*);

void task_request_handler(TaskHandlerData* data)
{
    Message msg;
    Error* err;

    TaggedTask* tagged_task = &data->task_request.tagged_task;

    // Tells client that the task is executing
    msg = create_task_status_msg(tagged_task->task_type, TASK_STATUS_EXECUTING);
    // Copies task request task
    msg.payload.task_status.tagged_task = *tagged_task;
    err = send_message(&msg, &data->client->task_connection);

    if (IS_NET_ERROR(err)) {
        print_error(err);
        free_error(err);
        err = CREATE_ERR_OK;
    }

    switch (tagged_task->task_type) {

    case TASK_USERS:
        err = server_task_users(data);
        break;

    case TASK_FILES:
        err = server_task_files(data);
        break;

    case TASK_CLIENT_UPLOAD_FILE:
        err = server_task_upload_file(data);
        break;

    case TASK_CLIENT_DOWNLOAD_FILE:
        err = server_task_download_file(data);
        break;

    default:
        printf("Unimplemented task %d\n", tagged_task->task_type);
        break;
    }

    if (IS_NET_ERROR(err)) {
        print_error(err);
        free_error(err);
        err = CREATE_ERR_OK;
    }

    free(data);
}
