#ifndef __CHAT_WINDOW_H__
#define __CHAT_WINDOW_H__
#include "ui/ui.h"

void ui_chat_window_create();
void ui_chat_window_free();
void ui_chat_window_resize();
void ui_chat_window_render();
void ui_chat_window_add_entry(ChatEntry* chat_entry);
void ui_chat_window_scroll_up();
void ui_chat_window_scroll_down();
void ui_chat_window_clear();

#endif