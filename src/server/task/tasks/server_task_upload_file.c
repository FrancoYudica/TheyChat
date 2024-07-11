#include "task/task_handler_data.h"
#include "net/net_communication.h"
#include "net/file_transfer.h"
#include "broadcast_message.h"
#include "file.h"

Error* server_task_upload_file(TaskHandlerData* data)
{
    Error* err;
    Message message;
    Server* server = get_server();
    Client* client = data->client;
    TaggedTask* tagged_task = &data->task_request.tagged_task;
    TaskFileUploadData* file_upload = &tagged_task->data.file_upload;

    // The filename is it's id
    pthread_mutex_lock(&server->shared_file_list_mutex);
    uint32_t next_file_id = shared_file_list_get_next_id(server->shared_file_list);
    pthread_mutex_unlock(&server->shared_file_list_mutex);
    char id_filename[10];
    sprintf(id_filename, "%i", next_file_id);

    // Receives file and places under resources folder
    uint64_t file_size = 0;
    err = receive_file(
        &data->client->task_connection,
        SHARED_FILES_LOCATION,
        id_filename,
        &file_size);

    if (IS_NET_ERROR(err))
        return err;

    pthread_mutex_lock(&server->shared_file_list_mutex);

    // Formats filepath as /resources/uploads/file_id
    char id_filepath[MAX_FILEPATH_SIZE];
    filepath_concat(id_filepath, SHARED_FILES_LOCATION, id_filename);

    // Adds shared file to the list
    SharedFile* file = shared_file_list_add(server->shared_file_list);
    shared_file_init(
        file,
        file_upload->filename,
        id_filepath,
        client->id,
        file_size);

    pthread_mutex_unlock(&server->shared_file_list_mutex);

    // Tells all clients that the file was successfully sent
    message = create_server_notification(
        "%s uploaded a file: \"%s\", id: %d of %d bytes",
        data->client->name,
        file->filename,
        file->id,
        file->size);

    send_broadcast(&message, server);

    return err;
}
