#ifndef __FILE_MESSAGE_H__
#define __FILE_MESSAGE_H__
#include "../message.h"

/// @brief Stores descriptive data of the file
typedef struct
{
    BaseMessage base;

    /// @brief Name of the file
    char name[64];

    /// @brief Size of the file in bytes
    uint32_t size;

} FileInfoMsg;

void init_file_info_message(FileInfoMsg *message, const char* name, uint32_t size);

#endif