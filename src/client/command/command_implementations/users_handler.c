#include "states_fsm.h"

Error* users_handler(uint8_t, char**)
{
    Message message;
    Client* client = get_client();
    // Gets header of the sequence
    Error* err = wait_for_message_type(
        &client->stream,
        client->connection_context,
        &message,
        MSGT_SEQUENCE_START);

    if (IS_NET_ERROR(err))
        return err;

    uint8_t message_type;

    while (true) {

        // Receives message
        err = wait_for_message(
            &client->stream,
            client->connection_context,
            &message);

        if (IS_NET_ERROR(err))
            return err;

        message_type = message.type;

        // If it's sequence, reads and then free
        if (message_type == MSGT_HEAP_SEQUENCE) {
            printf("NAME: %s\n", message.payload.heap_sequence.payload);
            free(message.payload.heap_sequence.payload);
            break;

        } else if (message_type == MSGT_SEQUENCE_END) {
            break;
        } else
            return CREATE_ERR(ERR_NET_RECEIVED_INVALID_TYPE, "Expected MSGT_HEAP_SEQUENCE or MSGT_SEQUENCE_END");
    }

    return err;
}