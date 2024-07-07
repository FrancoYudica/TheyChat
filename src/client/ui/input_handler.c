#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "ui/input_handler.h"
#include "ui/ui.h"
#include "string/utils.h"
#include "messages/message_types.h"

// Input callback, called after user entered input.
static Error* (*s_input_callback)(const char*);

// Input callback, called after user entered a command.
static Error* (*s_command_callback)(const char*);

static Error* s_err;
static pthread_t s_input_thread;

/// @brief Mutex used to block setters and getter on input functions
static pthread_mutex_t s_mutex;

/// @brief Flag used to tell input thread when application finishes
static bool s_running = false;

static void* handle_input()
{
    // Holds client input
    char input[MAX_CHAT_TEXT_BYTES];
    set_thread_name(pthread_self(), "input handler");

    while (s_running) {

        // Renders input window, and blocks current thread until input is received
        input[0] = '\0';
        ui_try_pop_input(input);

        if (strlen(input) == 0)
            continue;

        char* trimmed_input = trim(input);
        bool is_command = trimmed_input[0] == '/' && strlen(trimmed_input) > 1;

        // Locks when calling callback
        pthread_mutex_lock(&s_mutex);

        // Command callback
        if (is_command && s_command_callback != NULL)
            s_err = s_command_callback(input);

        // Usual input callback
        else if (s_input_callback != NULL)
            s_err = s_input_callback(input);

        pthread_mutex_unlock(&s_mutex);

        if (IS_NET_ERROR(s_err)) {
            ui_push_text_entry(
                TEXT_ENTRY_TYPE_WARNING,
                "%s",
                s_err->message);
        }
        usleep(50000); // Sleep for 5ms to prevent high CPU usage
    }
    unregister_thread(pthread_self());

    return NULL;
}

void input_handler_init()
{
    s_running = true;
    s_input_callback = NULL;
    pthread_create(&s_input_thread, NULL, handle_input, NULL);
    pthread_mutex_init(&s_mutex, NULL);
}

void input_handler_free()
{
    s_running = false;
    unregister_thread(s_input_thread);
    pthread_detach(s_input_thread);
    pthread_mutex_destroy(&s_mutex);
}

void input_handler_set_input_callback(Error* (*input_callback)(const char*))
{
    pthread_mutex_lock(&s_mutex);
    s_input_callback = input_callback;
    pthread_mutex_unlock(&s_mutex);
}

void input_handler_set_command_callback(Error* (*input_callback)(const char*))
{
    pthread_mutex_lock(&s_mutex);
    s_command_callback = input_callback;
    pthread_mutex_unlock(&s_mutex);
}
