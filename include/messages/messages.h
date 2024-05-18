#ifndef __MESSAGES_H__
#define __MESSAGES_H__
#include <stdlib.h>
#include "file_message.h"
#include "user_message.h"



uint32_t messages_sizes[] = {
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


#endif