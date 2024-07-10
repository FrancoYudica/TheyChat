#include "states_fsm.h"
#include "ui/ui.h"
#include "net/net_communication.h"
#include "file.h"

Error* download_handler(uint8_t argc, char** argv)
{
    Message message;
    Error* err;
    Client* client = get_client();

    // Ensures that the name is provided
    if (argc != 2) {
        ui_push_text_entry(
            TEXT_ENTRY_TYPE_WARNING,
            "Command \"/download\" requires {filename} argument as \"/download file.txt\"");
        return CREATE_ERR_OK;
    }

    const char* filename = argv[1];

    // Sends request to server
    message = create_task_request_msg(TASK_CLIENT_DOWNLOAD_FILE);
    TaskRequestPayload* request = &message.payload.task_request;
    TaskFileDownloadData* download_data = &request->tagged_task.data.file_download;

    // Sets filename
    strcpy(download_data->filename, filename);
    err = send_message(&message, &client->status_connection);
    return err;
}