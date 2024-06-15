#include "command/command_processor.h"
#include "net/net_communication.h"

ErrorCode execute_command_processor(ServerStateData* data, CommandMsg* command_message)
{
    printf("RECEIVED COMMAND MESSAGE!\n");

    switch (command_message->command_type) {
    case CMDT_DISCONNECT:
        return ERR_NET_DISCONNECT;
        break;

    default:
        break;
    }

    return ERR_NET_OK;
}