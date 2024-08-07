#ifndef __COMMAND_H__
#define __COMMAND_H__
#include <stdarg.h>
#include <stdbool.h>
#include "they_chat_error.h"

typedef Error* (*CommandHandler)(uint8_t argc, char** argv);

enum CommandType {
    CMD_NULL = 0,
    CMD_HELP,
    CMD_CONNECT,
    CMD_DISCONNECT,
    CMD_QUIT,
    CMD_USERS,
    CMD_UPLOAD,
    CMD_DOWNLOAD,
    CMD_FILES,
    CMD_REMOVE
};

typedef struct
{
    char* name;
    enum CommandType type;
    CommandHandler handler;
} Command;

/// @brief Given the input, find Command and executed if it's command type
/// is specified in the vargs, that should contain accepted CMD_TYPES
Error* dispatch_command(const char* input, uint32_t num_types, ...);

const Command* get_command_by_name(const char* command_name);

#endif