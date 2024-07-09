#include "server.h"
#include "task/task_handler_data.h"
#include "net/net_communication.h"
#include "server_state_data.h"

extern Error* server_task_users(TaskHandlerData*);
extern Error* server_task_upload_file(TaskHandlerData*);

void task_request_handler(TaskHandlerData* data)
{
    Message msg;
    Error* err;

    TaskRequestPayload* task_request = &data->task_request;

    msg = create_task_status_msg(task_request->task_type, TASK_STATUS_EXECUTING);
    err = send_message(&msg, &data->client->task_connection);

    if (IS_NET_ERROR(err)) {
        print_error(err);
        free_error(err);
    }

    switch (task_request->task_type) {

    case TASK_USERS:
        err = server_task_users(data);
        break;

    case TASK_CLIENT_UPLOAD_FILE:
        err = server_task_upload_file(data);
        break;

    default:
        printf("Unimplemented task %d\n", task_request->task_type);
        break;
    }

    if (IS_NET_ERROR(err)) {
        print_error(err);
        free_error(err);
    }

    free(data);
}
