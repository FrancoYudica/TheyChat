#ifndef __FILE_TRANSFER_H__
#define __FILE_TRANSFER_H__
#include <stdbool.h>
#include "they_chat_error.h"
#include "net/net_connection.h"

/// @brief Given the filepath and a network connection, sends a file.
/// @param override_name is an optional argument, that changes the sent file name
Error* send_file(
    const char* filepath,
    NetworkConnection* net_connection,
    const char* override_name);

/// @brief Given a network connection, receives the file and places under folder/filename.
/// Note that folder, filename and size are optional arguments, that accept NULL values.
/// Filename gets the file name if it's NULL.
/// Size gets the file size in bytes.
Error* receive_file(
    NetworkConnection* net_connection,
    const char* folder,
    char* filename,
    uint64_t* size);

#endif