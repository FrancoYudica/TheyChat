#ifndef __NET_ERROR_H__
#define __NET_ERROR_H__
#include <pthread.h>
#include <stdint.h>
#include <errno.h>

/// @brief Type used to retrieve status after network operations
typedef enum {
    ERR_OK = 0,
    ERR_OPEN_FILE,
    ERR_INVALID_ARGUMENT,

    // Network errors
    ERR_NET_SOCKET_CREATION_FAILED,
    ERR_NET_UNABLE_TO_CONNECT,
    ERR_NET_DISCONNECT,
    ERR_NET_FAILURE,
    ERR_NET_PEER_DISCONNECTED,
    ERR_NET_SEND_FAIL,
    ERR_NET_RECEIVE_FAIL,
    ERR_NET_RECEIVED_INVALID_TYPE,
    ERR_NET_STREAM_OVERFLOW,
    ERR_NET_ACCEPT_FAIL
} ErrorCode;

#define IS_NET_ERROR(X) ((X) != ERR_OK)
#define ASSERT_NET_ERROR(X) assert(!IS_NET_ERROR(X))

typedef struct
{
    ErrorCode code;
    const char* message;
    const char* errno_msg;
    const char* file;
    uint32_t line;
} Error;

Error* create_error(
    ErrorCode code,
    const char* message,
    const char* errno_msg,
    const char* file,
    int line);

void free_error(Error* error);

void print_error(const Error* err);

#define CREATE_ERROR(code, msg) create_error((code), (msg), NULL, __FILE__, __LINE__)
#define CREATE_ERROR_ERRNO(code, msg) create_error((code), (msg), strerror(errno), __FILE__, __LINE__)
#define CREATE_ERR_OK CREATE_ERROR(ERR_OK, NULL)

#endif