#include "states_fsm.h"
#include "ui/ui.h"
#include "net/net_communication.h"

Error* users_handler(uint8_t cmd_type, char**)
{
    Message message;
    Error* err;
    Client* client = get_client();

    message = create_task_request_msg(TASK_USERS, NULL);
    err = send_message(&message, &client->status_connection);
    return err;
}