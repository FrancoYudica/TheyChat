#include "server.h"
#include "command/command_handler_data.h"

extern void command_request_handler(CommandHandlerData* data)
{
    printf("Handling command!!!\n");

    free(data);
}
