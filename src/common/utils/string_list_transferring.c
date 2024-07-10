#include "utils/string_list_transferring.h"

Error* send_string_list(
    NetworkConnection* net_connection,
    StringList* list)
{
    Error* err;
    Message message;

    // Sequence start
    message = create_seq_start_msg();
    err = send_message(&message, net_connection);

    if (IS_NET_ERROR(err))
        return err;

    StringListIterator* it = string_list_iterator_create(list);
    char const* str = NULL;

    // Sends heap sequences
    while ((str = string_list_iterator_next(it))) {

        // Creates message and sends
        message = create_heap_seq_msg((const uint8_t*)str, strlen(str) + 1);
        err = send_message(&message, net_connection);

        // Frees heap memory
        free(message.payload.heap_sequence.payload);

        if (IS_NET_ERROR(err))
            break;
    }

    string_list_iterator_destroy(it);

    if (IS_NET_ERROR(err))
        return err;

    // Sequence end
    message = create_seq_end_msg();
    err = send_message(&message, net_connection);
    return err;
}

Error* receive_string_list(
    NetworkConnection* net_connection,
    StringList* list)
{
    Error* err;
    Message message;

    // Gets header of the sequence
    err = wait_for_message_type(
        net_connection,
        &message,
        MSGT_SEQUENCE_START);

    if (IS_NET_ERROR(err))
        return err;

    while (true) {

        // Receives message
        err = wait_for_message(
            net_connection,
            &message);

        if (IS_NET_ERROR(err))
            return err;

        enum MessageType message_type = message.type;

        // If it's sequence, adds to list and then free
        if (message_type == MSGT_HEAP_SEQUENCE) {
            string_list_add(list, (const char*)message.payload.heap_sequence.payload);
            free(message.payload.heap_sequence.payload);
            continue;

        } else if (message_type == MSGT_SEQUENCE_END) {
            break;
        } else
            return CREATE_ERR(ERR_NET_RECEIVED_INVALID_TYPE, "Expected MSGT_HEAP_SEQUENCE or MSGT_SEQUENCE_END");
    }

    return err;
}
