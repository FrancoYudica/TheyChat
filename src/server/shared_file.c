#include <string.h>
#include <time.h>

#include "shared_file.h"
#include "file.h"

void shared_file_init(
    SharedFile* file,
    const char* filename,
    const char* filepath,
    uint32_t client_id,
    uint32_t size)
{

    // Copies filename
    strncpy(
        file->filename,
        filename,
        sizeof(file->filename));

    // Copies server's filepath
    strncpy(
        file->filepath,
        filepath,
        sizeof(file->filepath));

    // Sets time to current
    file->shared_time = time(NULL);
    file->size = size;
    file->client_id = client_id;
}

Error* shared_file_remove_from_disk(SharedFile* file)
{
    if (!file_exists(file->filepath))
        return CREATE_ERR(ERR_FILE, "File doesn't exist");

    return file_remove(file->filepath);
}
