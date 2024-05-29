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
    uint8_t binary_payload[512];
} FileContentMsg;

typedef struct
{
    MessageHeader base;
} FileEndMsg;

void init_file_header_message(FileHeaderMsg* message, const char* name, uint32_t size);
void init_file_content_message(FileContentMsg* message, const uint8_t* content, uint32_t size);
void init_file_end_message(FileEndMsg* message);

#endif