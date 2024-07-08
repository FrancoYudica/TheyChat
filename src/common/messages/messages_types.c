#include "messages/message_types.h"

// Maps numeric type to string
static const char* message_types[] = {
    [MSGT_USER_CHAT] = "USER_CHAT",
    [MSGT_USER_LOGIN] = "USER_LOGIN",
    [MSGT_USER_LOGOUT] = "USER_LOGOUT",
    [MSGT_FILE_HEADER] = "FILE_HEADER",
    [MSGT_FILE_CONTENT] = "FILE_CONTENT",
    [MSGT_FILE_END] = "FILE_END",
    [MSGT_CLIENT_CONNECTED] = "CLIENT_CONNECTED",
    [MSGT_CLIENT_ON_QUEUE] = "CLIENT_ON_QUEUE",
    [MSGT_STATUS] = "STATUS",
    [MSGT_CONNECTED_CLIENTS] = "CONNECTED_CLIENTS",
    [MSGT_TASK_REQUEST] = "COMMAND",
    [MSGT_SEQUENCE_START] = "SEQUENCE_START",
    [MSGT_SEQUENCE_END] = "SEQUENCE_END",
    [MSGT_HEAP_SEQUENCE] = "HEAP_SEQUENCE"
};

const char* msg_get_type_name(enum MessageType type)
{
    size_t array_size = sizeof(message_types) / sizeof(message_types[0]);

    // When it's out of bounds, not implemented
    if (array_size < (size_t)(type + 1)) {
        printf("Unimplemented print_message for type %d\n", type);
        return "Unimplemented";
    }
    return message_types[type];
}