#include "task/task_handler_data.h"
#include "net/net_communication.h"
#include "string/string_list.h"
#include "utils/string_list_transferring.h"

Error* server_task_files(TaskHandlerData* data)
{
    Error* err;
    Server* server = data->server;
    SharedFileList* files = data->server->shared_file_list;

    pthread_mutex_lock(&data->server->shared_file_list_mutex);
    TaskFilesData* files_data = &data->task_request.tagged_task.data.files;

    char buffer[512];
    uint32_t space = 10;
    StringList* str_list = string_list_create();

    // Iterates through all clients and fills string list
    SharedFileListIterator* it = shared_file_list_iterator_create(server->shared_file_list);
    SharedFile* file;
    while ((file = shared_file_list_iterator_next(it))) {
        // Format the string with fixed-width fields
        sprintf(
            buffer,
            "ID: %-*i Name: %-*s Size: %-*li Sent by: %-*s",
            10, file->id,
            20, file->filename,
            10, file->size,
            10, file->client_name);

        string_list_add(str_list, buffer);
    }

    pthread_mutex_unlock(&data->server->shared_file_list_mutex);

    // Sends string list to client
    err = send_string_list(&data->client->task_connection, str_list);
    string_list_destroy(str_list);
    return err;
}
