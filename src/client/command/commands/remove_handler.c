#include "states_fsm.h"
#include "ui/ui.h"
#include "net/net_communication.h"

Error* remove_handler(uint8_t argc, char** argv)
{

    if (argc == 1) {
        ui_push_text_entry(
            TEXT_ENTRY_TYPE_WARNING,
            "Missing arguments while executing \"/remove\" command.");

        ui_set_log_text("Unable to execute /remove command");
        return CREATE_ERR_OK;
    }

    Message message;
    Error* err;
    Client* client = get_client();

    message = create_task_request_msg(TASK_REMOVE_FILE);
    TaskRequestPayload* request = &message.payload.task_request;
    TaskRemoveFileData* remove_data = &request->tagged_task.data.remove;

    remove_data->remove_all = !strcmp(argv[1], "all");

    if (!remove_data->remove_all)
        remove_data->file_id = atoi(argv[1]);

    err = send_message(&message, &client->status_connection);

    if (!IS_ERROR(err))
        ui_set_log_text("remove task request sent to server");

    return err;
}