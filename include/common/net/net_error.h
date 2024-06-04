#ifndef __NET_ERROR_H__
#define __NET_ERROR_H__
#include <pthread.h>
#include <stdint.h>

/// @brief Type used to retrieve status after network operations
typedef enum {
    ERR_NET_OK = 0,
    ERR_PEER_DISCONNECTED,
    ERR_SEND_FAIL,
    ERR_RECEIVE_FAIL,
    ERR_RECEIVED_INVALID_TYPE,
    ERR_NET_STREAM_OVERFLOW,
    ERR_OPEN_FILE,
    ERR_NET_ACCEPT_FAIL
} ErrorCode;

#define IS_NET_ERROR(X) ((X) != ERR_NET_OK)
#define ASSERT_NET_ERROR(X) assert(!IS_NET_ERROR(X))
#define IS_MAYOR_ERROR(X) ((X) != ERR_NET_OK && (X) != ERR_PEER_DISCONNECTED)

// #define ASSERT_NET_ERROR(X) (if (IS_NET_ERROR(X)) print_error())

typedef struct
{
    ErrorCode code;
    const char* message;
    const char* file;
    uint32_t line;
} NetError;

// Function prototypes
NetError create_error(ErrorCode code, const char* message, const char* file, int line);
void print_error(const NetError* err);

#define CREATE_ERROR(code, msg) create_error((code), (msg), __FILE__, __LINE__)

#endif