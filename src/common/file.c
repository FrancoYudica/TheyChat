#include "file.h"
#include <string.h>
#include <unistd.h>

const char* filepath_get_filename(const char* filepath)
{
    const char* filename = strrchr(filepath, PATH_SEPARATOR);
    return (filename != NULL) ? filename + 1 : filepath;
}

bool file_exists(const char* filepath)
{
    return access(filepath, F_OK) == 0;
}

bool file_can_read(const char* filepath)
{
    return access(filepath, R_OK) == 0;
}

bool file_can_write(const char* filepath)
{
    return access(filepath, W_OK) == 0;
}