#include "states_fsm.h"
#include "ui/ui.h"
#include "net/net_communication.h"
#include "utils/string_list_transferring.h"

Error* server_task_users(TaskStatusPayload status)
{
    Error* err;
    Client* client = get_client();

    ChatEntry* entry = chat_entry_create_list("Connected users:");
    err = receive_string_list(&client->task_connection, entry->data.list.list);

    if (!IS_ERROR(err))
        ui_add_chat_entry(entry);

    return err;
}
