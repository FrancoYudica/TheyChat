#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "file.h"

const char* filepath_get_filename(const char* filepath)
{
    const char* filename = strrchr(filepath, PATH_SEPARATOR[0]);
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
Error* file_remove(const char* filepath)
{
    if (remove(filepath) == 0) {
        return CREATE_ERR_OK;
    }
    return CREATE_ERRNO(ERR_FILE, "Error while removing file");
}

void filepath_concat(
    char* dest,
    const char* folder,
    const char* file)
{
    sprintf(
        dest,
        "%s%s%s",
        folder,
        PATH_SEPARATOR,
        file);
}
