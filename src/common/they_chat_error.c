#include "they_chat_error.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

Error* _create_error(
    ErrorCode code,
    const char* message,
    const char* errno_msg,
    const char* file,
    int line)
{
    Error* error = (Error*)malloc(sizeof(Error));
    if (error == NULL) {
        return NULL;
    }
    error->code = code;
    error->message = message ? strdup(message) : NULL;
    error->errno_msg = errno_msg ? strdup(errno_msg) : NULL;
    error->file = file ? strdup(file) : NULL;
    error->line = line;
    return error;
}

void free_error(Error* error)
{
    if (error) {
        if (error->message)
            free((void*)error->message);
        if (error->errno_msg)
            free((void*)error->errno_msg);
        if (error->file)
            free((void*)error->file);
        free(error);
    }
}

void print_error(const Error* err)
{
    if (err == NULL)
        printf("Null err\n");

    else if (err->code != ERR_OK) {
        fprintf(
            stderr,
            "Error: %s\n Errno msg: %s\n Code: %d\nFile: %s\nLine: %d\n",
            err->message,
            err->errno_msg,
            err->code,
            err->file,
            err->line);
    }
}

void _assert_net_error(Error* err)
{
    if (IS_NET_ERROR(err)) {
        print_error(err);
        free_error(err);
        exit(EXIT_FAILURE);
    }
}
