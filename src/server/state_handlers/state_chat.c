#include <pthread.h>
#include "state_handler_utils.h"
#include "broadcast_message.h"
#include "task/task_handler_data.h"

extern void task_request_handler(TaskHandlerData* data);

Error* handle_state_chat(ServerStateData* state_data, AppState* next_state)
{
    Client* client = state_data->client;

    Error* err = CREATE_ERR_OK;
    Message message;
    while (true) {

        err = wait_for_message(&client->status_connection, &message);

        if (IS_NET_ERROR(err)) {

            if (err->code == ERR_NET_CONNECTION_CLOSED)
                break;

            return err;
        }

        // Outputs client message
        print_message(&message);

        // Broadcasts the message to all clients
        if (message.type == MSGT_USER_CHAT) {
            strcpy(message.payload.user_chat.ip, client->ip);
            send_broadcast((const Message*)&message, state_data->server);
        }

        else if (message.type == MSGT_SERVER_CMD_REQUEST) {
            TaskHandlerData* handler_data = malloc(sizeof(TaskHandlerData));
            handler_data->client = client;
            handler_data->server = state_data->server;
            handler_data->task_request = message.payload.task_request;
            thpool_submit(
                state_data->server->cmd_thread_pool,
                (thread_task_t)task_request_handler,
                handler_data);
        }

        else
            printf("Only MSGT_USER_CHAT and MSGT_SERVER_CMD_REQUEST should be received in chat handler. There is something wrong...\n");

        if (IS_NET_ERROR(err))
            break;
    }

    *next_state = APP_STATE_DISCONNECT;
    return err;
}