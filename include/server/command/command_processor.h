#ifndef __COMMAND_PROCESSOR_H__
#define __COMMAND_PROCESSOR_H__
#include "they_chat_error.h"
#include "server_state_data.h"
#include "messages/message_types.h"

Error* execute_command_processor(ServerStateData* data, Message* command_message);

#endif