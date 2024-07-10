#include "task/task_handler_data.h"
#include "net/net_communication.h"
#include "net/file_transfer.h"
#include "file.h"

Error* server_task_download_file(TaskHandlerData* data)
{
    Error* err;
    Message message;
    TaggedTask* tagged_task = &data->task_request.tagged_task;
    TaskFileDownloadData* download_data = &tagged_task->data.file_download;

    const char* filename = download_data->filename;
    printf("Client download request: %s\n", filename);

    char filepath[MAX_FILEPATH_SIZE];
    filepath_concat(filepath, "resources", filename);

    // If the file doesn't exists, tells the client
    if (!file_exists(filepath)) {
        message = create_status_msg(
            false,
            "Unable to download file. File named \"%s\" doesn't exist",
            filename);
        err = send_message(&message, &data->client->task_connection);
        return err;
    }

    // Sends confirmation
    message = create_status_msg(true, NULL);
    err = send_message(&message, &data->client->task_connection);

    err = send_file(filepath, &data->client->task_connection);

    return err;
}
