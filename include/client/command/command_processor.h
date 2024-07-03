#ifndef __COMMAND_PROCESSOR_H__
#define __COMMAND_PROCESSOR_H__
#include "they_chat_error.h"
#include "client.h"

Error* execute_command_processor(Client* data, uint8_t command_type, const char* command_arg);

#endif