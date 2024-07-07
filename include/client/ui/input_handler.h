#ifndef __INPUT_HANDLER_H__
#define __INPUT_HANDLER_H__
#include "they_chat_error.h"

void input_handler_init();
void input_handler_free();
void input_handler_set_input_callback(Error* (*input_callback)(const char*));
void input_handler_set_command_callback(Error* (*input_callback)(const char*));

#endif