#include "states_fsm.h"
#include "ui/ui.h"
#include "net/net_communication.h"
#include "utils/string_list_transferring.h"

Error* server_task_files(TaskStatusPayload status)
{
    Error* err;
    Client* client = get_client();

    ChatEntry* entry = chat_entry_create_list("Files:");
    err = receive_string_list(&client->task_connection, entry->data.list.list);

    if (!IS_ERROR(err)) {

        if (string_list_length(entry->data.list.list) == 0) {
            chat_entry_free(entry);
            ui_push_text_entry(TEXT_ENTRY_TYPE_SERVER, "There are no files uploaded");
        } else
            ui_add_chat_entry(entry);
    }

    return err;
}
