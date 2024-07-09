#include "task/task_handler_data.h"
#include "net/net_communication.h"

Error* server_task_upload_file(TaskHandlerData* data)
{
    Error* err;
    Message message;
    TaggedTask* tagged_task = &data->task_request.tagged_task;
    printf("Receiving file: %s\n", tagged_task->data.file_upload.filename);
    return err;
}
