#ifndef __UI_DATA_H__
#define __UI_DATA_H__
#include <ncurses.h>
#include <pthread.h>
#include "messages/message.h"

typedef struct {

    uint32_t white_color_pair;
    uint32_t soft_color_pair;
    uint32_t name_color_pair;

    // Mutexes
    pthread_mutex_t render_mutex;

} UI;

#endif