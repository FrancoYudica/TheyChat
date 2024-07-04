#include "command/command.h"
#include "ui/ui.h"

// Help handler is different, requires variable argument list
static void help_handler(uint32_t, va_list);

Error* dispatch_command(const char* input, uint32_t cmd_count, ...)
{
    // Parse input into argc and argv
    char* input_copy = strdup(input); // Make a copy of the input
    char* token = strtok(input_copy, " ");
    char* argv[10];
    uint8_t argc = 0;
    while (token != NULL && argc < 10) {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }

    Error* err = CREATE_ERR_OK;

    // Gets command
    const Command* command = get_command_by_name(argv[0]);

    if (command == NULL) {
        ui_set_log_text("Unrecognized command \"%s\"", argv[0]);
        return err;
    }

    // Tests if command type is specified in vargs
    va_list args;
    va_start(args, cmd_count);

    if (command->type == CMD_HELP) {
        help_handler(cmd_count, args);
    } else {
        for (uint32_t i = 0; i < cmd_count; i++) {

            uint32_t type = va_arg(args, uint32_t);
            if (type == command->type) {
                err = command->handler(argc, argv);
                va_end(args);
                return err;
            }
        }
    }
    va_end(args);
    ui_set_log_text("Can't execute command: \"%s\" in this context", argv[0]);
    return err;
}

typedef struct
{
    char* name;
    char* description;
} CommandDefinition;

static void help_handler(uint32_t cmd_count, va_list args)
{

    static CommandDefinition command_help[] = {
        [CMD_HELP] = { "\"/h\" or \"/help\"", "Lists all the commands" },
        [CMD_CONNECT] = { "\"/c\" or \"/connect\"", "Establishes connection with server. First argument, specifies server ip, and second, the port, both are optional" },
        [CMD_DISCONNECT] = { "\"/d\" or \"/disconnect\"", "Disconnects from server, and transitions to offline state" },
        [CMD_QUIT] = { "\"/q\" or \"/quit\"", "Quits the application" },
        [CMD_USERS] = { "\"/users\"", "Lists all connected users" }
    };

    ui_push_text_entry(TEXT_ENTRY_TYPE_LOG, "Listing commands:");

    uint32_t command_width = 25;
    for (uint32_t i = 0; i < cmd_count; i++) {
        enum CommandType cmd_type = va_arg(args, uint32_t);

        ui_push_text_entry(
            TEXT_ENTRY_TYPE_LOG,
            "   %-*s - %s",
            command_width,
            command_help[cmd_type].name,
            command_help[cmd_type].description);
    }
}

static Error* unimplemented_handler(uint8_t argc, char** argv)
{
    ui_set_log_text("Unimplemented command \"%s\"", argv[0]);
    return CREATE_ERR_OK;
}
extern Error* disconnect_handler(uint8_t, char**);
extern Error* connect_handler(uint8_t, char**);
extern Error* quit_handler(uint8_t, char**);

// Lookup table that stores all the application commands
static Command s_commands[] = {
    { "/help", CMD_HELP, unimplemented_handler },
    { "/h", CMD_HELP, unimplemented_handler },
    { "/connect", CMD_CONNECT, connect_handler },
    { "/c", CMD_CONNECT, connect_handler },
    { "/disconnect", CMD_DISCONNECT, disconnect_handler },
    { "/d", CMD_DISCONNECT, disconnect_handler },
    { "/quit", CMD_QUIT, unimplemented_handler },
    { "/q", CMD_QUIT, unimplemented_handler },
    { "/users", CMD_USERS, unimplemented_handler },
    { "/u", CMD_USERS, unimplemented_handler }
};

const Command* get_command_by_name(const char* command_name)
{
    uint32_t num_commands = sizeof(s_commands) / sizeof(Command);

    for (uint32_t i = 0; i < num_commands; i++) {
        if (!strcmp(s_commands[i].name, command_name))
            return &s_commands[i];
    }
    return NULL;
}