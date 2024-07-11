#include "server.h"
#include "file.h"
#include "messages/message_types.h"
#include "broadcast_message.h"

static Server s_server;

Server* get_server()
{
    return &s_server;
}

void server_client_count_update()
{
    // Sends to all clients the currently connected clients
    uint32_t connected_client_count = client_list_length(s_server.client_list);
    Message connected_clients = create_connected_clients_msg(connected_client_count);
    send_broadcast((const Message*)&connected_clients, get_server());
}

Error* server_remove_shared_file(uint32_t id)
{
    // Finds in list
    SharedFile* file = shared_file_list_find_by_id(s_server.shared_file_list, id);
    if (file == NULL)
        return CREATE_ERR(ERR_FILE, "Unable to find specified file id in shared file list");

    // Removes from disk
    Error* err = shared_file_remove_from_disk(file);
    if (IS_NET_ERROR(err))
        return err;

    // Removes from list
    bool removed = shared_file_list_remove(s_server.shared_file_list, file->id);
    if (!removed)
        return CREATE_ERR(ERR_FILE, "Trying to remove file from shared_file_list, when it doesn't exists");

    return CREATE_ERR_OK;
}
