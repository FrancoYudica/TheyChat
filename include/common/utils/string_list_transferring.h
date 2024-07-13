#ifndef __STRING_LIST_TRANSFERRING_H__
#define __STRING_LIST_TRANSFERRING_H__
#include "utils/string_list.h"
#include "they_chat_error.h"
#include "net/net_communication.h"

Error* send_string_list(
    NetworkConnection* net_connection,
    StringList* send_list);

Error* receive_string_list(
    NetworkConnection* net_connection,
    StringList* received_list);

#endif
