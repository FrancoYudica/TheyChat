#ifndef __UI_DATA_H__
#define __UI_DATA_H__
#include <ncurses.h>
#include <pthread.h>
#include "messages/message.h"

enum UI_COLOR_PAIR {

    // Bar color pairs
    COLOR_PAIR_BAR = 1,
    COLOR_PAIR_BAR_ENABLED,
    COLOR_PAIR_BAR_DISABLED,

    // Chat color pairs
    COLOR_PAIR_CHAT,
    COLOR_PAIR_CHAT_USERNAME,
    COLOR_PAIR_CHAT_ALTERNATIVE,

    COLOR_PAIR_CHAT_TEXT,
    COLOR_PAIR_CHAT_SERVER, // Server notifications
    COLOR_PAIR_CHAT_WARNING
};

typedef struct {
    // Mutexes
    pthread_mutex_t render_mutex;

} UI;

#endif