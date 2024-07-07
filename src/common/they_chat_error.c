#include "they_chat_error.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

static const char* s_error_names[] = {
    [ERR_OK] = "ERR_OK",
    [ERR_OPEN_FILE] = "ERR_OPEN_FILE",
    [ERR_CLOSE_FD] = "ERR_CLOSE_FD",
    [ERR_NET_SOCKET_CREATION_FAILED] = "ERR_NET_SOCKET_CREATION_FAILED",
    [ERR_NET_UNABLE_TO_CONNECT] = "ERR_NET_UNABLE_TO_CONNECT",
    [ERR_NET_FAILURE] = "ERR_NET_FAILURE",
    [ERR_NET_PEER_DISCONNECTED] = "ERR_NET_PEER_DISCONNECTED",
    [ERR_NET_CONNECTION_CLOSED] = "ERR_NET_CONNECTION_CLOSED",
    [ERR_NET_RECEIVED_INVALID_TYPE] = "ERR_NET_RECEIVED_INVALID_TYPE",
    [ERR_NET_STREAM_OVERFLOW] = "ERR_NET_STREAM_OVERFLOW"
};

Error* _create_error(
    ErrorCode code,
    const char* message,
    const char* errno_msg,
    const char* file,
    int line)
{
    Error* error = (Error*)malloc(sizeof(Error));
    if (error == NULL)
        return NULL;

    error->code = code;

    if (errno_msg == NULL)
        snprintf(
            error->message,
            sizeof(error->message),
            "Thread (%li) error: [%d] [%s]:\"%s\" in [%s]:[%i]",
            pthread_self(),
            code,
            s_error_names[code],
            message,
            file,
            line);
    else
        snprintf(
            error->message,
            sizeof(error->message),
            "Error: [%d] %s [%s]:\"%s\" in [%s]:[%i]",
            code,
            errno_msg,
            s_error_names[code],
            message,
            file,
            line);

    return error;
}

void free_error(Error* error)
{
    if (error)
        free(error);
}

void print_error(const Error* err)
{
    if (err == NULL)
        printf("No error\n");

    printf("%s", err->message);
}

void _assert_net_error(Error* err)
{
    if (IS_NET_ERROR(err)) {
        print_error(err);
        free_error(err);
        exit(EXIT_FAILURE);
    }
}
