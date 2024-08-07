// Includes all the common dependencies of the state handlers
#ifndef __CLIENT_STATE_HANDLER_UTILS_H__
#define __CLIENT_STATE_HANDLER_UTILS_H__
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#include "messages/message_types.h"
#include "application_states.h"
#include "states_fsm.h"
#include "net/net_communication.h"
#include "they_chat_error.h"
#include "client.h"
#include "ui/chat_entry_types.h"
#include "ui/ui.h"
#include "ui/input_handler.h"
#include "command.h"
#include "utils/string_utils.h"

#endif