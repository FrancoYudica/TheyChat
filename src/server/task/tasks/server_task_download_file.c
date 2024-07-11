#include "task/task_handler_data.h"
#include "net/net_communication.h"
#include "net/file_transfer.h"
#include "file.h"

Error* server_task_download_file(TaskHandlerData* data)
{
    Error* err;
    Message message;
    Server* server = data->server;
    TaggedTask* tagged_task = &data->task_request.tagged_task;
    TaskFileDownloadData* download_data = &tagged_task->data.file_download;

    SharedFile* file = NULL;

    // When the unique file ID is specified
    if (download_data->by_id) {
        uint32_t file_id = atoi(download_data->filename);
        file = shared_file_list_find_by_id(server->shared_file_list, file_id);
    }
    // When the not unique filename is used
    else {
        uint32_t filename_count = shared_file_list_count_name(server->shared_file_list, download_data->filename);

        // When there are multiple files under the same name
        if (filename_count > 1) {
            message = create_status_msg(
                false,
                "There are multiple files named \"%s\". Use \"/download id {file_id}\" instead. Note that the id are displayed using \"/files\" command",
                download_data->filename);
            return send_message(&message, &data->client->task_connection);
        }
        // When there is just one file with that name
        else if (filename_count == 1)
            file = shared_file_list_find_by_name(server->shared_file_list, download_data->filename);
    }

    // When file isn't in memory
    if (file == NULL) {
        message = create_status_msg(
            false,
            "Unable to download file. File named \"%s\" doesn't exist in memory",
            download_data->filename);
        return send_message(&message, &data->client->task_connection);
    }

    // The filepath is resources/file_id
    char filepath[MAX_FILEPATH_SIZE];
    sprintf(filepath, "resources%c%i", PATH_SEPARATOR, file->id);

    // If the file doesn't exists, tells the client
    if (!file_exists(filepath)) {
        message = create_status_msg(
            false,
            "Unable to download file. File named \"%s\" doesn't exist in disk",
            file->filename);
        return send_message(&message, &data->client->task_connection);
    }

    // Sends confirmation
    message = create_status_msg(true, NULL);
    err = send_message(&message, &data->client->task_connection);

    if (IS_NET_ERROR(err))
        return err;

    return send_file(
        filepath,
        &data->client->task_connection,
        file->filename);
}
