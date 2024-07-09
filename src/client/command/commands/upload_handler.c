#include "states_fsm.h"
#include "ui/ui.h"
#include "net/net_communication.h"
#include "file.h"

Error* upload_handler(uint8_t argc, char** argv)
{
    Message message;
    Error* err;
    Client* client = get_client();

    // Ensures that the name is provided
    if (argc != 2) {
        ui_push_text_entry(
            TEXT_ENTRY_TYPE_WARNING,
            "Command /upload requires {filepath} argument as \"/upload file.txt\"");
        return CREATE_ERR_OK;
    }

    const char* filepath = argv[1];

    // Ensures that the file exists
    if (!file_exists(filepath)) {
        ui_push_text_entry(
            TEXT_ENTRY_TYPE_WARNING,
            "Couldn't find filepath: \"%s\". Check that the filepath is absolute, or relative to client executable",
            filepath);
        return CREATE_ERR_OK;
    }

    // Ensures that the file is readable
    if (!file_can_read(filepath)) {
        ui_push_text_entry(
            TEXT_ENTRY_TYPE_WARNING,
            "Client doesn't have pesmisions to read the provided file: \"%s\"",
            filepath);
        return CREATE_ERR_OK;
    }

    // Sends request to server
    message = create_task_request_msg(TASK_CLIENT_UPLOAD_FILE);
    TaskRequestPayload* request = &message.payload.task_request;
    TaskFileUploadData* upload_data = &request->tagged_task.data.file_upload;

    const char* filename = filepath_get_filename(filepath);
    strcpy(upload_data->filename, filename);
    err = send_message(&message, &client->status_connection);
    return err;
}