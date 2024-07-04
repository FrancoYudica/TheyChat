#ifndef __INPUT_HANDLER_H__
#define __INPUT_HANDLER_H__
#include "they_chat_error.h"
#include "pthread.h"

typedef struct
{
    // Input callback, called after user entered input.
    Error* (*input_callback)(const char*);

    // Input callback, called after user entered a command.
    Error* (*command_callback)(const char*);
    void* user_data;
    Error* error;
    pthread_t input_thread;

    /// @brief Mutex used to block setters and getter on input functions
    pthread_mutex_t mutex;

} InputHandlerData;

void input_handler_init();
void input_handler_free();
void input_handler_set_input_callback(Error* (*input_callback)(const char*));
void input_handler_set_command_callback(Error* (*input_callback)(const char*));

#endif