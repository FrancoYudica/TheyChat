#include "states_fsm.h"
#include "ui/ui.h"
#include "net/net_communication.h"
#include "net/file_transfer.h"

Error* server_task_upload_file(TaskStatusPayload status)
{
    Message message;
    Error* err = CREATE_ERR_OK;
    Client* client = get_client();
    err = send_file("/home/franco/file", &client->task_connection);
    return err;
}