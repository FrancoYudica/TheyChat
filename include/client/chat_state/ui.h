#ifndef __UI_H__
#define __UI_H__

#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "messages/message.h"
// Forward declaration of ChatEntries
typedef struct chat_entries ChatEntries;

typedef struct
{
    WINDOW* header_window;
    WINDOW* input_window;
    WINDOW* chat_window;
    WINDOW* log_window;

    // Header window data
    char server_ip[24];
    uint32_t connected_count;

    // Chat window data
    ChatEntries* chat_entries;
    uint32_t chat_entries_offset;

    // Log window data
    char log[64];

    // Input window data
    bool input_enabled;

    char input_text[MAX_CHAT_TEXT_BYTES];

    // Mutexes
    pthread_mutex_t render_mutex;

} UI;

void ui_init(UI* ui);
void ui_add_chat_entry(UI* ui, const char* name, const char* text);
void ui_set_connected_users(UI* ui, uint32_t count);
void ui_free(UI* ui);

#endif
