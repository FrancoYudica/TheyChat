#include "task/task_handler_data.h"
#include "net/net_communication.h"
#include "net/file_transfer.h"

Error* server_task_upload_file(TaskHandlerData* data)
{
    Error* err;
    Message message;
    TaggedTask* tagged_task = &data->task_request.tagged_task;
    printf("Receiving file: %s\n", tagged_task->data.file_upload.filename);
    err = receive_file(&data->client->task_connection, "resources", NULL);

    if (IS_NET_ERROR(err)) {
        printf("Error during file receive\n");
    } else {
        printf("File received successfully\n");
    }

    return err;
}
