#ifndef __SHARED_FILE_H__
#define __SHARED_FILE_H__
#include <stdint.h>
#include <time.h>
#include "constants.h"

/// @brief Represents a file shared by a client
typedef struct
{
    /// @brief Unique file ID
    uint32_t id;
    char filename[MAX_FILENAME_SIZE];
    uint64_t size;
    /// @brief Time when the file was shared
    time_t shared_time;

    // Client data
    uint32_t client_id;
    char client_name[MAX_USERNAME_BYTES];
} SharedFile;

#endif