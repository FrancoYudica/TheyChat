#include "command/command.h"
#include "command/command_implementations.h"
#include "ui/ui.h"

Error* dispatch_command(const char* input, uint32_t num_types, ...)
{
    // Parse input into argc and argv
    char* input_copy = strdup(input); // Make a copy of the input
    char* token = strtok(input_copy, " ");
    char* argv[10];
    uint32_t argc = 0;
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
    va_start(args, input);

    for (uint32_t i = 0; i < num_types; i++) {

        uint32_t type = va_arg(args, uint32_t);
        if (type == command->type) {
            err = command->handler(argc, argv);
            va_end(args);
            return err;
        }
    }
    va_end(args);
    ui_set_log_text("Can't execute command: \"%s\" in this context", argv[0]);
    return err;
}
