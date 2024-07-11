#include "task/task_handler_data.h"
#include "net/net_communication.h"
#include "string/string_list.h"
#include "utils/string_list_transferring.h"

Error* server_task_remove_file(TaskHandlerData* data)
{
    Error* err;
    Message message;
    Server* server = data->server;
    Client* client = data->client;
    TaskRemoveFileData* remove_file_data = &data->task_request.tagged_task.data.remove;

    // Removes all client files
    if (remove_file_data->remove_all) {
        SharedFile* file = NULL;
        SharedFileListIterator* it = shared_file_list_iterator_create(server->shared_file_list);

        // Removes all the files that belong to the client
        uint32_t removed_count = 0;
        while ((file = shared_file_list_iterator_next(it))) {
            if (file->client_id == client->id) {

                // Tries to remove
                err = server_remove_shared_file(file->id);

                // Displays error
                if (IS_NET_ERROR(err)) {
                    print_error(err);
                    free_error(err);
                    err = CREATE_ERR_OK;
                } else
                    removed_count++;
            }
        }

        shared_file_list_iterator_destroy(it);

        // If no files where removed
        if (removed_count == 0) {
            message = create_status_msg(
                false,
                "User %s haven't uploaded ant file",
                client->name);
        } else {
            message = create_status_msg(
                true,
                "Removed %d %s's files",
                removed_count,
                client->name);
        }
        return send_message(&message, &data->client->task_connection);

    }
    // Removes the file specified by ID
    else {
        SharedFile* file = shared_file_list_find_by_id(
            server->shared_file_list,
            remove_file_data->file_id);

        // Not found
        if (file == NULL) {
            message = create_status_msg(
                false,
                "There isn't any file of id: \"%d\"",
                remove_file_data->file_id);
            return send_message(&message, &data->client->task_connection);
        }

        // Client doesn't have permissions to remove this file
        if (file->client_id != client->id) {
            message = create_status_msg(
                false,
                "You don't have permissions to remove file of id: \"%d\" since it's not yours",
                file->id);
            return send_message(&message, &data->client->task_connection);
        }

        char filename[MAX_FILENAME_SIZE];
        strcpy(filename, file->filename);

        // Tries to remove file
        err = server_remove_shared_file(file->id);
        bool removed = !IS_NET_ERROR(err);

        // Displays error
        if (!removed) {
            print_error(err);
            free_error(err);
            err = CREATE_ERR_OK;
        }

        // Creates status message
        if (removed) {
            message = create_status_msg(
                true,
                "File \"%s\" removed successfully",
                filename);
        } else {
            message = create_status_msg(
                false,
                "Unable to remove \"%s\"",
                filename);
        }
        // Sends status to client
        return send_message(&message, &data->client->task_connection);
    }

    return CREATE_ERR_OK;
}
