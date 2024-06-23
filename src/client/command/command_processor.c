#include "command/command_processor.h"
#include "messages/message_types.h"
#include "net/net_communication.h"

Error* process_users_command(ClientData* data)
{
    Message message;

    // Gets header of the sequence
    Error* err = wait_for_message_type(
        &data->stream,
        data->connection_context,
        &message,
        MSGT_SEQUENCE_START);

    if (IS_NET_ERROR(err))
        return err;

    uint8_t message_type;

    while (true) {

        // Receives message
        err = wait_for_message(
            &data->stream,
            data->connection_context,
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

Error* execute_command_processor(ClientData* data, uint8_t command_type, const char* command_arg)
{
    // Sends command msg to server, telling that a command arrived
    Message message = create_command_msg(command_type, command_arg);
    Error* err = send_message((const Message*)&message, data->connection_context);

    if (IS_NET_ERROR(err))
        return err;

    switch (command_type) {
    case CMDT_DISCONNECT:
        return CREATE_ERR(ERR_NET_DISCONNECT, "Disconnecting with command");

    case CMDT_USERS:
        return process_users_command(data);

    default:
        break;
    }

    return err;
}