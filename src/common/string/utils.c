#include "string/utils.h"
#include <string.h>
#include <ctype.h>

char* trim(char* str)
{
    char* end;

    // Trim leading space
    while (isspace((unsigned char)*str))
        str++;

    if (*str == 0) { // All spaces?
        return str;
    }

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;

    // Write new null terminator
    *(end + 1) = '\0';

    return str;
}

char* split_string_inplace(char* str, char delimiter)
{
    if (!str) {
        return NULL;
    }

    // Find the position of the delimiter
    char* delim_pos = strchr(str, delimiter);
    if (!delim_pos) {
        return NULL; // Delimiter not found
    }

    // Replace the delimiter with a null terminator to split the string
    *delim_pos = '\0';

    // Return a pointer to the second part of the split string
    return delim_pos + 1;
}

bool starts_with(const char* str, const char* prefix)
{
    if (str == NULL || prefix == NULL) {
        return false;
    }

    size_t str_len = strlen(str);
    size_t prefix_len = strlen(prefix);

    // If the string is shorter than the prefix, it can't start with it
    if (str_len < prefix_len) {
        return false;
    }

    // Compare the prefix with the beginning of the string
    return strncmp(str, prefix, prefix_len) == 0;
}