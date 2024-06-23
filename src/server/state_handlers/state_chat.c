#include <pthread.h>
#include "state_handler_utils.h"
#include "broadcast_message.h"
#include "command/command_processor.h"

Error* handle_state_chat(ServerStateData* state_data, AppState* next_state)
{
    Client* client = state_data->client;

    Error* err = CREATE_ERR_OK;
    Message message;
    while (true) {

        err = wait_for_message(&client->stream, client->connection_context, &message);

        if (IS_NET_ERROR(err))
            return err;

        // Outputs client message
        print_message(&message);

        // Broadcasts the message to all clients
        if (message.type == MSGT_USER_CHAT) {
            strcpy(message.payload.user_chat.ip, client->ip);
            send_broadcast((const Message*)&message, state_data->server);
        }

        // Process command
        else if (message.type == MSGT_COMMAND)
            err = execute_command_processor(state_data, &message);

        else
            printf("Only MSGT_USER_CHAT and MSGT_COMMAND should be received in chat handler. There is something wrong...\n");

        if (IS_NET_ERROR(err))
            break;
    }

    *next_state = APP_STATE_DISCONNECT;
    return err;
}