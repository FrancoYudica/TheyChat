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
    Error* messages_error;
} Chat;

void chat_exit(Chat* chat);

#endif