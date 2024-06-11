#ifndef __UI_H__
#define __UI_H__

#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "messages/message.h"
#include "chat_entries.h"
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

    uint32_t white_color_pair;
    uint32_t soft_color_pair;
    uint32_t name_color_pair;

    // Log window data
    char log[256];

    // Input window data
    bool input_enabled;

    char input_text[MAX_CHAT_TEXT_BYTES];

    // Mutexes
    pthread_mutex_t render_mutex;

} UI;

void ui_init(UI* ui);
void ui_add_chat_entry(UI* ui, ChatEntry entry);
void ui_set_log_text(UI* ui, const char* text);
void ui_refresh(UI* ui);
void ui_free(UI* ui);

#endif
