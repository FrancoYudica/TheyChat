#include <pthread.h>
#include "state_handler_utils.h"
#include "broadcast_message.h"
#include "command/command_processor.h"

ErrorCode handle_state_chat(ServerStateData* state_data, AppState* next_state)
{
    Client* client = state_data->client;

    ErrorCode error = ERR_NET_OK;
    while (true) {

        // Waits for any message
        Message* client_msg;
        ErrorCode receive_status = wait_for_message(&client->stream, client->connection_context, &client_msg);

        if (IS_NET_ERROR(receive_status)) {
            if (receive_status != ERR_PEER_DISCONNECTED)
                printf("NETWORK ERROR type %i while handling client %d\n", receive_status, client->id);

            error = receive_status;
            break;
        }
        // Outputs client message
        print_message(client_msg);

        // Broadcasts the message to all clients
        if (client_msg->header.type == MSGT_USER_CHAT) {
            UserChatMsg* client_chat_message = (UserChatMsg*)client_msg;

            // Creates a new user chat msg with server time
            UserChatMsg* server_chat_msg = create_user_chat_msg(
                client_chat_message->text,
                client_chat_message->user_base.username);

            // Copies client IP
            strncpy(server_chat_msg->ip, client->ip, sizeof(server_chat_msg->ip));

            send_broadcast((const Message*)server_chat_msg, state_data->server);

            free(server_chat_msg);
        }

        // Process command
        else if (client_msg->header.type == MSGT_COMMAND) {
            error = execute_command_processor(state_data, (CommandMsg*)client_msg);
        }

        else {
            printf("Only MSGT_USER_CHAT and MSGT_COMMAND should be received in chat handler. There is something wrong...\n");
        }

        free(client_msg);

        if (IS_NET_ERROR(error))
            break;
    }

    *next_state = APP_STATE_DISCONNECT;
    return error;
}