#ifndef __NET_ERROR_H__
#define __NET_ERROR_H__
#include <pthread.h>
#include <stdint.h>
#include <errno.h>

/// @brief Type used to retrieve status after network operations
typedef enum {
    ERR_OK = 0,
    ERR_OPEN_FILE,
    ERR_CLOSE_FD,

    // Network errors
    ERR_NET_SOCKET_CREATION_FAILED,
    ERR_NET_UNABLE_TO_CONNECT,
    ERR_NET_FAILURE,
    ERR_NET_PEER_DISCONNECTED,
    ERR_NET_CONNECTION_CLOSED,
    ERR_NET_RECEIVED_INVALID_TYPE,
    ERR_NET_STREAM_OVERFLOW
} ErrorCode;

#define IS_NET_ERROR(X) ((X) != NULL)

typedef struct
{
    ErrorCode code;
    char message[1024];
} Error;

Error* _create_error(
    ErrorCode code,
    const char* message,
    const char* errno_msg,
    const char* file,
    int line);

void free_error(Error* error);

void print_error(const Error* err);

void set_thread_name(pthread_t tid, const char* name);

const char* get_thread_name(pthread_t tid);

void unregister_thread(pthread_t tid);

void _assert_net_error(Error* err);

#define ASSERT_NET_ERROR(X) _assert_net_error(X)

// Creates an error without using the errno variable
#define CREATE_ERR(code, msg) _create_error((code), (msg), NULL, __FILE__, __LINE__)

// Crates an error using errno variable
#define CREATE_ERRNO(code, msg) _create_error((code), (msg), strerror(errno), __FILE__, __LINE__)

// Creates OK error, as NULL Error
#define CREATE_ERR_OK (Error*)NULL

#endif