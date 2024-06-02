#ifndef __FILE_MESSAGE_H__
#define __FILE_MESSAGE_H__
#include "message.h"

/// @brief Stores descriptive data of the file
typedef struct
{
    MessageHeader base;

    /// @brief Name of the file
    char name[64];

    /// @brief Size of the file in bytes
    uint32_t size;

} FileHeaderMsg;

typedef struct
{
    MessageHeader base;

    /// @brief Size in bytes of the actual content. This is necessary
    /// since the last FileContentMsg won't fill the entire binary_payload buffer
    uint32_t content_size;
    uint8_t binary_payload[512];
} FileContentMsg;

typedef struct
{
    MessageHeader base;
} FileEndMsg;

FileHeaderMsg* create_file_header_message(const char* name, uint32_t size);
FileContentMsg* create_file_content_message(const uint8_t* content, uint32_t size);
FileEndMsg* create_file_end_message();

#endif