#ifndef __COMMAND_PROCESSOR_H__
#define __COMMAND_PROCESSOR_H__
#include "net/net_error.h"
#include "client_data.h"

ErrorCode execute_command_processor(ClientData* data, uint8_t command_type, const char* command_arg);

#endif