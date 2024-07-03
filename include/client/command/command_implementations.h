#ifndef __COMMAND_IMPLEMENTATIONS_H__
#define __COMMAND_IMPLEMENTATIONS_H__
#include "they_chat_error.h"
#include "command/command.h"

typedef Error* (*CommandHandler)(uint32_t argc, char** argv);

const Command* get_command_by_name(const char* command_name);

#endif