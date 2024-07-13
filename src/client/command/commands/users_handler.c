#include "states_fsm.h"
#include "ui/ui.h"
#include "net/net_communication.h"

Error* users_handler(uint8_t argc, char** argv)
{
    Message message;
    Error* err;
    Client* client = get_client();

    message = create_task_request_msg(TASK_USERS);
    TaskRequestPayload* request = &message.payload.task_request;
    TaskUsersDada* users_data = &request->tagged_task.data.users;

    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "ip"))
            users_data->show_ip = true;

        if (!strcmp(argv[i], "id"))
            users_data->show_id = true;
    }

    err = send_message(&message, &client->status_connection);

    if (!IS_ERROR(err))
        ui_set_log_text("users task request sent to server");

    return err;
}