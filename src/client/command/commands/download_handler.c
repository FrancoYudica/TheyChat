#include "states_fsm.h"
#include "ui/ui.h"
#include "net/net_communication.h"
#include "file.h"

Error* download_handler(uint8_t argc, char** argv)
{
    Message message;
    Error* err;
    Client* client = get_client();

    const char* filename;
    bool by_id = false;

    // Ensures that the name is provided
    if (argc == 1) {
        ui_push_text_entry(
            TEXT_ENTRY_TYPE_ERROR,
            "Command \"/download\" requires {filename} argument as \"/download file.txt\"");

        ui_set_log_text("Check /download command format");
        return CREATE_ERR_OK;
    }

    if (argc == 2)
        filename = argv[1];
    else if (argc == 3) {
        filename = argv[2];

        if (strcmp(argv[1], "id")) {
            ui_push_text_entry(
                TEXT_ENTRY_TYPE_ERROR,
                "Invalid argument \"%s\"",
                argv[1]);

            ui_set_log_text("Check /download command format");
            return CREATE_ERR_OK;
        }

        by_id = true;
    } else {
        ui_push_text_entry(
            TEXT_ENTRY_TYPE_ERROR,
            "Invalid \"/download\" command format");

        ui_set_log_text("Check /download command format");
        return CREATE_ERR_OK;
    }

    // Sends request to server
    message = create_task_request_msg(TASK_CLIENT_DOWNLOAD_FILE);
    TaskRequestPayload* request = &message.payload.task_request;
    TaskFileDownloadData* download_data = &request->tagged_task.data.file_download;
    download_data->by_id = by_id;

    // Sets filename
    strcpy(download_data->filename, filename);
    err = send_message(&message, &client->status_connection);

    if (!IS_ERROR(err))
        ui_set_log_text("download task request sent to server");

    return err;
}