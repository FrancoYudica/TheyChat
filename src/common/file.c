#include "file.h"
#include <string.h>

const char* filepath_get_filename(const char* filepath)
{
    const char* filename = strrchr(filepath, PATH_SEPARATOR);
    return (filename != NULL) ? filename + 1 : filepath;
}
