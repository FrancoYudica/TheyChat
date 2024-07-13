#include "states_fsm.h"
#include "ui/ui.h"
#include "net/net_communication.h"
#include "net/file_transfer.h"
#include "file.h"

#define DOWNLOADS_FOLDER_PATH "resources/downloads"

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

    if (IS_ERROR(err))
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
    ui_set_log_text("Downloading file: \"%s\"", download_data->filename);
    char received_filename[MAX_FILENAME_SIZE];
    uint64_t downloaded_bytes = 0;
    err = receive_file(
        &client->task_connection,
        DOWNLOADS_FOLDER_PATH,
        received_filename,
        &downloaded_bytes);

    // If there isn't any error, tells client
    if (IS_ERROR(err)) {
        ui_push_text_entry(
            TEXT_ENTRY_TYPE_ERROR,
            "Unable to download file");

    } else {
        ui_push_text_entry(
            TEXT_ENTRY_TYPE_SERVER,
            "Downloaded %d bytes! File located in \"%s%c%s\"",
            downloaded_bytes,
            DOWNLOADS_FOLDER_PATH,
            PATH_SEPARATOR,
            received_filename);
    }
    return err;
}