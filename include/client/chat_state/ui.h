#ifndef __UI_H__
#define __UI_H__

#include <stdlib.h>
#include "chat_entries.h"

void ui_init();
void ui_add_chat_entry(ChatEntry entry);
void ui_set_log_text(const char* text);
void ui_refresh();
void ui_free();
void ui_set_connected_count(uint32_t count);
void ui_set_server_ip(const char* ip);
void ui_try_pop_input(char* output_buffer);

#endif
