#ifndef __SHARED_FILE_H__
#define __SHARED_FILE_H__
#include <stdint.h>
#include <time.h>
#include "constants.h"
#include "they_chat_error.h"

#define SHARED_FILES_LOCATION "resources" PATH_SEPARATOR "uploads"

/// @brief Represents a file shared by a client
typedef struct
{
    /// @brief Unique file ID
    uint32_t id;

    // Server relative filepath. As /resources/uploads/file_id
    char filepath[MAX_FILEPATH_SIZE];

    // filename of client's computer
    char filename[MAX_FILENAME_SIZE];

    // Size in bytes
    uint64_t size;

    /// @brief Time when the file was shared
    time_t shared_time;

    // Client data
    uint32_t client_id;
} SharedFile;

/// @brief Initializes all file attributes
void shared_file_init(
    SharedFile* file,
    const char* filename,
    const char* filepath,
    uint32_t client_id,
    uint32_t size);

Error* shared_file_remove_from_disk(SharedFile* file);

#endif