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

    if (IS_NET_ERROR(err))
        return err;

    Server* server = data->server;
    Client* client = data->client;

    // Adds shared file to the list
    SharedFile* file = shared_file_list_add(server->shared_file_list);
    strcpy(file->filename, file_upload->filename);
    file->shared_time = time(NULL);
    file->client_id = client->id;
    strcpy(file->client_name, client->name);

    // Tells all clients that the file was successfully sent
    message = create_server_notification(
        "%s uploaded a file: \"%s\", id: %d",
        data->client->name,
        file->filename,
        file->id);

    send_broadcast(&message, data->server);

    return err;
}
