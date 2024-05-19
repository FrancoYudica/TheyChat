#include "message_types.h"


static uint32_t messages_sizes[] = {
    sizeof(UserChatMsg),
    sizeof(UserLoginMsg),
    0,
    sizeof(FileInfoMsg)
};

uint32_t msg_get_type_size(uint8_t type)
{
    uint8_t arr_len = sizeof(messages_sizes) / sizeof(uint32_t);
    if (type >= arr_len)
    {
        printf("`msg_get_type_size` Index out of bounds!");
        exit(1);
    }
    uint32_t size = messages_sizes[type];
    if (size == 0)
    {
        printf("`msg_get_type_size` Unimplemented type size for type %i\n", type);
        exit(1);
    }
    return size;
}

const char* msg_get_type_name(uint8_t type)
{
    // Maps numeric type to string
    static const char* message_types[] = {
        "CHAT",
        "LOGIN",
        "LOGOUT",
        "FILE"
    };


    size_t array_size = sizeof(message_types) / sizeof(message_types[0]);

    // When it's out of bounds, not implemented
    if (array_size < (size_t)(type + 1))
    {
        printf("Unimplemented print_message for type %d\n", type);
        return "Unimplemented";
    }
    return message_types[type];
}