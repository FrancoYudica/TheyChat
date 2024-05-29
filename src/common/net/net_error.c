#include "net/net_error.h"
#include <stdio.h>

NetError create_error(ErrorCode code, const char* message, const char* file, int line)
{
    NetError error = { code, message, file, line };
    return error;
}

void print_error(const NetError* err)
{
    if (err->code != ERR_NET_OK) {
        fprintf(
            stderr,
            "Error: %s\nCode: %d\nFile: %s\nLine: %d\n",
            err->message,
            err->code,
            err->file,
            err->line);
    }
}
