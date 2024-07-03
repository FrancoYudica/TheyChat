#ifndef __LOG_WINDOW_H__
#define __LOG_WINDOW_H__
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

void ui_log_window_create();
void ui_log_window_free();
void ui_log_window_resize();
void ui_log_window_set_log(const char* format, ...);
void ui_log_window_render();

#endif