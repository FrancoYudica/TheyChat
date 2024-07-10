#include "states_fsm.h"
#include "ui/ui.h"
#include "net/net_communication.h"
#include "net/file_transfer.h"
#include "file.h"

Error* server_task_download_file(TaskStatusPayload status)
{
    Message message;
    Error* err = CREATE_ERR_OK;
    Client* client = get_client();
    TaggedTask* tagged_task = &status.tagged_task;
    TaskFileDownloadData* download_data = &tagged_task->data.file_download;

    // Wais for server status
    err = wait_for_message_type(
        &client->task_connection,
        &message,
        MSGT_STATUS);

    if (IS_NET_ERROR(err))
        return err;

    // If status is false, it probably means that the
    // file doesn't exist
    if (!message.payload.status.status) {
        ui_push_text_entry(
            TEXT_ENTRY_TYPE_SERVER,
            "%s",
            message.payload.status.text);

        return CREATE_ERR_OK;
    }

    // Downloads file
    ui_set_log_text("Downloading file...");
    err = receive_file(
        &client->task_connection,
        "resources",
        NULL,
        NULL);

    // If there isn't any error, tells client
    if (!IS_NET_ERROR(err)) {
        ui_push_text_entry(
            TEXT_ENTRY_TYPE_SERVER,
            "File downloaded! Located in \"resources\" folder",
            download_data->filename,
            PATH_SEPARATOR,
            download_data->filename);
    }
    return err;
}