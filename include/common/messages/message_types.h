#ifndef __MESSAGES_TYPES_H__
#define __MESSAGES_TYPES_H__

#include <stdlib.h>
#include "user_message.h"
#include "file_message.h"
#include "client_message.h"
#include "status_message.h"
#include "command_message.h"
#include "seq_message.h"

/// @return Size in memory of the Message type
uint32_t msg_get_type_size(uint8_t type);

/// @return String name of the Message
const char* msg_get_type_name(uint8_t type);

#endif