#include <pthread.h>
#include "state_handler_utils.h"
#include "broadcast_message.h"
#include "command/command_processor.h"

ErrorCode handle_state_chat(ServerStateData* state_data, AppState* next_state)
{
    Client* client = state_data->client;

    ErrorCode error = ERR_NET_OK;
    Message message;
    while (true) {

        ErrorCode receive_status = wait_for_message(&client->stream, client->connection_context, &message);

        if (IS_NET_ERROR(receive_status)) {
            if (receive_status != ERR_PEER_DISCONNECTED)
                printf("NETWORK ERROR type %i while handling client %d\n", receive_status, client->id);

            error = receive_status;
            break;
        }
        // Outputs client message
        print_message(&message);

        // Broadcasts the message to all clients
        if (message.type == MSGT_USER_CHAT) {

            // Creates a new user chat msg with server time
            Message server_chat_msg = create_user_chat_msg(
                message.payload.user_chat.text,
                message.payload.user_chat.username);

            // Copies client IP
            strncpy(server_chat_msg.payload.user_chat.ip, client->ip, sizeof(server_chat_msg.payload.user_chat.ip));

            send_broadcast((const Message*)&server_chat_msg, state_data->server);

        }

        // Process command
        else if (message.type == MSGT_COMMAND)
            error = execute_command_processor(state_data, &message);

        else
            printf("Only MSGT_USER_CHAT and MSGT_COMMAND should be received in chat handler. There is something wrong...\n");

        if (IS_NET_ERROR(error))
            break;
    }

    *next_state = APP_STATE_DISCONNECT;
    return error;
}