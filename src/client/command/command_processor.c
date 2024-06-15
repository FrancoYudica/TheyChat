#include "command/command_processor.h"
#include "messages/message_types.h"
#include "net/net_communication.h"

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

    default:
        break;
    }

    return ERR_NET_OK;
}