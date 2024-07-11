#include "states_fsm.h"
#include "ui/ui.h"
#include "net/net_communication.h"
#include "utils/string_list_transferring.h"

Error* server_task_remove_file(TaskStatusPayload status)
{
    Error* err = CREATE_ERR_OK;
    Message message;
    Client* client = get_client();
    TaggedTask* tagged_task = &status.tagged_task;

    // Wais for server status
    err = wait_for_message_type(
        &client->task_connection,
        &message,
        MSGT_STATUS);

    if (IS_NET_ERROR(err))
        return err;

    StatusPayload* status_data = &message.payload.status;

    ui_push_text_entry(
        TEXT_ENTRY_TYPE_SERVER,
        "%s",
        status_data->text);

    return CREATE_ERR_OK;
}
