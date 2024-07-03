#include "command/command_implementations.h"
#include "command/command.h"
#include "ui/ui.h"
#include "states_fsm.h"

static Error* unimplemented_handler(uint8_t, char**);
static Error* help_handler(uint8_t, char**);
static Error* disconnect_handler(uint8_t, char**);
static Error* connect_handler(uint8_t, char**);
static Error* quit_handler(uint8_t, char**);

// Array that stores all the application commands
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

static Error* unimplemented_handler(uint8_t argc, char** argv)
{
    ui_set_log_text("Unimplemented command \"%s\"", argv[0]);
    return CREATE_ERR_OK;
}

static Error* help_handler(uint8_t, char**)
{
    return CREATE_ERR_OK;
}

static Error* disconnect_handler(uint8_t, char**)
{
    state_handler_set_next(APP_STATE_DISCONNECT);
    return CREATE_ERR_OK;
}

Error* connect_handler(uint8_t argc, char** argv)
{
    Client* data = get_client();

    // Localhost with default port
    if (argc == 1) {
        strcpy(data->connection_details.server_ip, "127.0.0.1");
        data->connection_details.port = 8000;
    }
    // Default port
    else if (argc == 2) {
        strcpy(data->connection_details.server_ip, argv[1]);
        data->connection_details.port = 8000;
    }
    // Specifies IP and port
    else {
        strcpy(data->connection_details.server_ip, argv[1]);
        data->connection_details.port = atoi(argv[2]);
    }

    state_handler_set_next(APP_STATE_CONNECT);
    return CREATE_ERR_OK;
}

static Error* quit_handler(uint8_t, char**)
{
    return CREATE_ERR_OK;
}