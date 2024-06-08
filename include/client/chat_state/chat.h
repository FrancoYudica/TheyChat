#ifndef __CHAT_DATA_H__
#define __CHAT_DATA_H__
#include <pthread.h>
#include <stdbool.h>
#include "net/net_error.h"
#include "client_data.h"

typedef struct
{

    ClientData* client_data;

    ErrorCode input_error;
    ErrorCode messages_error;
    pthread_t input_thread, messages_thread;
    pthread_mutex_t mutex;
    pthread_mutex_t condition_mutex;
    pthread_cond_t exit_condition;
    bool active;
} ChatData;

void chat_exit(ChatData* chat);

#endif