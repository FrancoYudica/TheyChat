#include "states_fsm.h"
#include "ui/ui.h"
#include "net/net_communication.h"

Error* files_handler(uint8_t argc, char** argv)
{
    Message message;
    Error* err;
    Client* client = get_client();

    message = create_task_request_msg(TASK_FILES);
    TaskRequestPayload* request = &message.payload.task_request;
    TaskFilesData* files_data = &request->tagged_task.data.files;
    err = send_message(&message, &client->status_connection);

    if (!IS_ERROR(err))
        ui_set_log_text("files task request sent to server");

    return err;
}