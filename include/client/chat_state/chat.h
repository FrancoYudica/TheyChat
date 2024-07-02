#ifndef __CHAT_DATA_H__
#define __CHAT_DATA_H__
#include <pthread.h>
#include <stdbool.h>
#include "they_chat_error.h"
#include "client_data.h"
#include "ui/ui.h"

typedef struct
{

    ClientData* client_data;
    Error* input_error;
    Error* messages_error;
    pthread_t input_thread, messages_thread;
    pthread_mutex_t mutex;
    pthread_mutex_t condition_mutex;
    pthread_cond_t exit_condition;
    bool active;
} Chat;

void chat_exit(Chat* chat);

#endif