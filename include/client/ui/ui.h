#ifndef __UI_H__
#define __UI_H__

#include <stdlib.h>
#include "chat_entries.h"

void ui_init();
void ui_add_chat_entry(ChatEntry entry);
void ui_set_log_text(const char* format, ...);
void ui_refresh();
void ui_free();
void ui_set_connected(bool is_connected);
void ui_set_tls_enabled(bool tls_enabled);
void ui_set_connected_count(uint32_t count);
void ui_set_server_ip(const char* ip);
void ui_try_pop_input(char* output_buffer);
void ui_set_input_prompt(const char* text);

enum TextEntryType {
    TEXT_ENTRY_TYPE_LOG,
    TEXT_ENTRY_TYPE_SERVER,
    TEXT_ENTRY_TYPE_WARNING,
    TEXT_ENTRY_TYPE_ERROR
};

void ui_push_text_entry(enum TextEntryType type, const char* format, ...);
#endif
