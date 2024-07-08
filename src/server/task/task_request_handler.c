#include "server.h"
#include "task/task_handler_data.h"
#include "net/net_communication.h"
#include "server_state_data.h"

extern Error* server_task_users(TaskHandlerData* data);

void task_request_handler(TaskHandlerData* data)
{
    Message msg;
    Error* err;

    msg = create_status_msg(true, "Task received successfully");
    err = send_message(&msg, &data->client->task_connection);

    if (IS_NET_ERROR(err)) {
        print_error(err);
        free_error(err);
    }

    TaskRequestPayload* task_request = &data->task_request;

    switch (task_request->task_type) {

    case TASK_USERS:
        // server_task_users(data);
        break;

    default:
        break;
    }

    free(data);
}
