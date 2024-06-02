#include <pthread.h>
#include "state_handler_utils.h"
#include "broadcast_message.h"

ErrorCode handle_state_chat(ServerStateData* state_data, AppState* next_state)
{
    Client* client = state_data->client;

    UserChatMsg* msg = create_user_chat_msg("Hey!", "SERVER");
    ErrorCode error = ERR_NET_OK;
    while (true) {
        // ErrorCode send_status = send_message((const Message*)msg, client->sockfd);

        // if (IS_NET_ERROR(send_status)) {
        //     if (send_status != ERR_PEER_DISCONNECTED)
        //         printf("NETWORK ERROR type %i while handling client %d\n", send_status, client->id);

        //     error = send_status;
        //     break;
        // }

        // Waits for any message
        Message* client_msg;
        ErrorCode receive_status = wait_for_message(&client->stream, client->sockfd, &client_msg);

        if (IS_NET_ERROR(receive_status)) {
            if (receive_status != ERR_PEER_DISCONNECTED)
                printf("NETWORK ERROR type %i while handling client %d\n", receive_status, client->id);

            error = receive_status;
            break;
        }
        // Outputs client message
        print_message(client_msg);

        // Broadcasts the message to all clients
        send_broadcast((const Message*)client_msg, state_data);

        free(client_msg);
    }

    free(msg);
    *next_state = APP_STATE_DISCONNECT;
    return error;
}