#include <unistd.h>
#include <pthread.h>
#include "state_handler_utils.h"

static bool in_chat = true;
static ErrorCode input_error;
static ErrorCode messages_error;
static pthread_t input_thread, messages_thread;

void* handle_messages(void* arg)
{
    ClientData* data = (ClientData*)arg;
    while (in_chat) {
        Message* server_message;
        messages_error = wait_for_message(&data->stream, data->sockfd, &server_message);

        if (IS_NET_ERROR(messages_error))
            break;

        print_message(server_message);
        free(server_message);
    }
    in_chat = false;
    return NULL;
}

void* handle_input(void* arg)
{
    ClientData* data = (ClientData*)arg;
    char input_buffer[MAX_CHAT_TEXT_BYTES];

    while (in_chat) {
        fgets(input_buffer, MAX_CHAT_TEXT_BYTES, stdin);

        if (strlen(input_buffer) == 0)
            continue;

        // Replaces \n with null termination
        input_buffer[strlen(input_buffer) - 1] = '\0';

        if (input_buffer[0] != '/') {
            UserChatMsg* msg = create_user_chat_msg(input_buffer, data->username);
            input_error = send_message((const Message*)msg, data->sockfd);
            free(msg);
        }

        else if (!strcmp(input_buffer, "/disconnect")) {
            printf("Disconnecting\n");
            break;
        }

        if (IS_NET_ERROR(input_error))
            break;
    }
    in_chat = false;
    return NULL;
}

ErrorCode handle_state_chat(ClientData* data, AppState* next_state)
{
    // Creates thread for handling input and receiving messages from server
    pthread_create(&input_thread, NULL, handle_input, data);
    pthread_create(&messages_thread, NULL, handle_messages, data);

    pthread_join(messages_thread, NULL);
    pthread_detach(input_thread);

    *next_state = APP_STATE_DISCONNECT;

    if (IS_NET_ERROR(input_error))
        return input_error;
    if (IS_NET_ERROR(messages_error))
        return messages_error;

    return ERR_NET_OK;
}