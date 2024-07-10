#include "task/task_handler_data.h"
#include "net/net_communication.h"
#include "net/file_transfer.h"
#include "broadcast_message.h"

Error* server_task_upload_file(TaskHandlerData* data)
{
    Error* err;
    Message message;
    TaggedTask* tagged_task = &data->task_request.tagged_task;
    TaskFileUploadData* file_upload = &tagged_task->data.file_upload;

    // Receives file and places under resources folder
    err = receive_file(&data->client->task_connection, "resources", NULL);

    if (IS_NET_ERROR(err)) {
        printf("Error during file receive\n");
    } else {

        // Tells all clients that the file was successfully sent
        message = create_server_notification(
            "Client named %s uploaded a file: \"%s\"",
            data->client->name,
            file_upload->filename);
        send_broadcast(&message, data->server);
    }

    return err;
}
