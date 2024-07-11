#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "they_chat_error.h"
#include "collections/linked_list.h"

static const char* s_error_names[] = {
    [ERR_OK] = "ERR_OK",
    [ERR_FILE] = "ERR_FILE",
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
            "Thread (%s) error: [%d] [%s]: \"%s\" in [%s]:[%i]",
            get_thread_name(pthread_self()),
            code,
            s_error_names[code],
            message,
            file,
            line);
    else
        snprintf(
            error->message,
            sizeof(error->message),
            "Thread (%s) error: [%d] %s [%s]: \"%s\" in [%s]:[%i]",
            get_thread_name(pthread_self()),
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
    if (IS_ERROR(err)) {
        print_error(err);
        free_error(err);
        exit(EXIT_FAILURE);
    }
}

typedef struct {
    pthread_t tid;
    char name[64];
} ThreadInfo;

static LinkedList* s_thread_names_list = NULL;

void set_thread_name(pthread_t tid, const char* name)
{
    if (s_thread_names_list == NULL)
        s_thread_names_list = linked_list_create(sizeof(ThreadInfo));

    ThreadInfo* info = (ThreadInfo*)linked_list_add(s_thread_names_list);
    info->tid = tid;
    strcpy(info->name, name);
}

const char* get_thread_name(pthread_t tid)
{
    if (s_thread_names_list == NULL)
        return "Unnamed";

    LinkedListIterator* it = linked_list_iterator_create(s_thread_names_list);
    ThreadInfo* info = NULL;
    const char* name = NULL;
    while ((info = linked_list_iterator_next(it))) {
        if (info->tid == tid) {
            name = info->name;
            break;
        }
    }
    linked_list_iterator_destroy(it);

    if (name == NULL)
        return "Unnamed";

    return name;
}

void unregister_thread(pthread_t tid)
{
    if (s_thread_names_list == NULL)
        return;

    LinkedListIterator* it = linked_list_iterator_create(s_thread_names_list);
    ThreadInfo* info = NULL;
    uint32_t i = 0;
    while ((info = linked_list_iterator_next(it))) {
        if (info->tid == tid) {
            linked_list_remove(s_thread_names_list, i);
            break;
        }
        i++;
    }
    linked_list_iterator_destroy(it);
}
