#ifndef __FILE_TRANSFER_H__
#define __FILE_TRANSFER_H__
#include <stdbool.h>
#include "they_chat_error.h"
#include "net/net_connection.h"

Error* send_file(const char* filepath, NetworkConnection* net_connection);
Error* receive_file(
    NetworkConnection* net_connection,
    const char* folder,
    const char* override_name,
    uint64_t* size);

#endif