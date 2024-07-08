#include "states_fsm.h"
#include "ui/ui.h"
#include "net/net_communication.h"

Error* server_task_users(uint8_t cmd_type, char**)
{
    Message message;
    Error* err;
    Client* client = get_client();

    // Gets header of the sequence
    err = wait_for_message_type(
        &client->task_connection,
        &message,
        MSGT_SEQUENCE_START);

    if (IS_NET_ERROR(err))
        return err;

    enum MessageType message_type;
    ChatEntry* entry = chat_entry_create_list("Connected users:");
    while (true) {

        // Receives message
        err = wait_for_message(
            &client->task_connection,
            &message);

        if (IS_NET_ERROR(err))
            return err;

        message_type = message.type;

        // If it's sequence, reads and then free
        if (message_type == MSGT_HEAP_SEQUENCE) {
            string_list_add(entry->data.list.list, message.payload.heap_sequence.payload);
            free(message.payload.heap_sequence.payload);
            break;

        } else if (message_type == MSGT_SEQUENCE_END) {
            break;
        } else
            return CREATE_ERR(ERR_NET_RECEIVED_INVALID_TYPE, "Expected MSGT_HEAP_SEQUENCE or MSGT_SEQUENCE_END");
    }
    ui_add_chat_entry(entry);
    return err;
}