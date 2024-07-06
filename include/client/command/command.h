#ifndef __COMMAND_H__
#define __COMMAND_H__
#include <stdarg.h>
#include <stdbool.h>
#include "they_chat_error.h"
#include "command_types.h"

typedef Error* (*CommandHandler)(uint8_t argc, char** argv);

typedef struct
{
    char* name;
    enum CommandType type;
    bool is_server_cmd;
    CommandHandler handler;
} Command;

/// @brief Given the input, find Command and executed if it's command type
/// is specified in the vargs, that should contain accepted CMD_TYPES
Error* dispatch_command(const char* input, uint32_t num_types, ...);

const Command* get_command_by_name(const char* command_name);

#endif