#include "states_fsm.h"
#include "ui/ui.h"
#include "net/net_communication.h"
#include "net/file_transfer.h"
#include "file.h"

Error* server_task_upload_file(TaskStatusPayload status)
{
    Message message;
    Error* err = CREATE_ERR_OK;
    Client* client = get_client();
    TaggedTask* tagged_task = &status.tagged_task;
    TaskFileUploadData* upload_data = &tagged_task->data.file_upload;

    const char* filepath = tagged_task->data.file_upload.user_filepath;

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
            "Client doesn't have permissions to read the provided file: \"%s\"",
            filepath);
        return CREATE_ERR_OK;
    }

    err = send_file(filepath, &client->task_connection, NULL);
    return err;
}