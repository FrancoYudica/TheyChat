#include "command/command_processor.h"
#include "messages/message_types.h"
#include "net/net_communication.h"

ErrorCode process_users_command(ClientData* data)
{
    Message* msg;

    // Gets header of the sequence
    ErrorCode err = wait_for_message_type(
        &data->stream,
        data->connection_context,
        &msg, MSGT_SEQUENCE_START);

    if (IS_NET_ERROR(err))
        return err;

    free(msg);

    uint8_t message_type;

    while (true) {

        // Receives message
        err = wait_for_message(
            &data->stream,
            data->connection_context,
            &msg);

        if (IS_NET_ERROR(err))
            return err;

        message_type = msg->header.type;

        // If it's sequence, reads and then free
        if (message_type == MSGT_HEAP_SEQUENCE) {
            HeapSequenceMsg* heap_seq = (HeapSequenceMsg*)msg;
            printf("NAME: %s\n", heap_seq->payload);
            free(heap_seq->payload);
            free(heap_seq);
            break;

        } else if (message_type == MSGT_SEQUENCE_END) {
            free(msg);
            break;
        } else
            return ERR_RECEIVED_INVALID_TYPE;
    }

    return ERR_NET_OK;
}

ErrorCode execute_command_processor(ClientData* data, uint8_t command_type, const char* command_arg)
{
    // Sends command msg to server, telling that a command arrived
    CommandMsg* command_msg = create_command_msg(command_type, command_arg);
    ErrorCode err = send_message((const Message*)command_msg, data->connection_context);

    if (IS_NET_ERROR(err))
        return err;

    free(command_msg);

    switch (command_type) {
    case CMDT_DISCONNECT:
        return ERR_NET_DISCONNECT;
        break;

    case CMDT_USERS: {

        err = process_users_command(data);

        break;
    }

    default:
        break;
    }

    return err;
}