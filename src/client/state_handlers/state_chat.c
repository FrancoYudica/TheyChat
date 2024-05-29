#include <unistd.h>
#include "state_handler_utils.h"

ErrorCode handle_state_chat(ClientData *data, AppState *next_state)
{

    Message *message = (Message*)create_user_chat_msg("Received message!", data->username);

    ErrorCode status = ERR_NET_OK;

    while(true)
    {
        Message *server_message;
        status = wait_for_message(&data->stream, data->sockfd, &server_message);

        if (IS_NET_ERROR(status))
            break;

        print_message(server_message);
        free(server_message);
        
        send_message((const Message*)message, data->sockfd);
        sleep(1);
    }
    free(message);

    *next_state = APP_STATE_DISCONNECT;
    return status;

}