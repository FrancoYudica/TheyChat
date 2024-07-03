#include <string.h>
#include <unistd.h>
#include "ui/input_handler.h"
#include "ui/ui.h"
#include "string/utils.h"
#include "pthread.h"
#include "messages/message_types.h"

static InputHandlerData s_input_data;

static void* handle_input()
{
    // Holds client input
    char input[MAX_CHAT_TEXT_BYTES];

    while (true) {

        usleep(1000); // Sleep for 20ms to prevent high CPU usage

        // Renders input window, and blocks current thread until input is received
        input[0] = '\0';
        ui_try_pop_input(input);

        if (strlen(input) == 0)
            continue;

        char* trimmed_input = trim(input);
        bool is_command = trimmed_input[0] == '/' && strlen(trimmed_input) > 1;

        Error* err = CREATE_ERR_OK;

        // Locks when calling callback
        pthread_mutex_lock(&s_input_data.mutex);

        // Command callback
        if (is_command && s_input_data.command_callback != NULL) {
            err = s_input_data.command_callback(input);
        }
        // Usual input callback
        else if (s_input_data.input_callback != NULL) {
            err = s_input_data.input_callback(input);
        }
        pthread_mutex_unlock(&s_input_data.mutex);

        if (IS_NET_ERROR(err)) {
            s_input_data.error = err;
            break;
        }
    }

    /// @TODO propagate the error to the application

    return NULL;
}

void input_handler_init()
{
    s_input_data.input_callback = NULL;
    s_input_data.user_data = NULL;
    s_input_data.error = NULL;
    pthread_create(&s_input_data.input_thread, NULL, handle_input, NULL);
    pthread_mutex_init(&s_input_data.mutex, NULL);
}

void input_handler_set_input_callback(Error* (*input_callback)(const char*))
{
    pthread_mutex_lock(&s_input_data.mutex);
    s_input_data.input_callback = input_callback;
    pthread_mutex_unlock(&s_input_data.mutex);
}

void input_handler_set_command_callback(Error* (*input_callback)(const char*))
{
    pthread_mutex_lock(&s_input_data.mutex);
    s_input_data.command_callback = input_callback;
    pthread_mutex_unlock(&s_input_data.mutex);
}
