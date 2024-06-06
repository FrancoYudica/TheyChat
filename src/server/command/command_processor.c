#include "command/command_processor.h"
#include "net/net_communication.h"

ErrorCode execute_command_processor(ServerStateData* data, CommandMsg* command_message)
{
    printf("RECEIVED COMMAND MESSAGE!\n");
    return ERR_NET_OK;
}