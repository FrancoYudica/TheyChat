#ifndef __INPUT_WINDOW_H__
#define __INPUT_WINDOW_H__
#include <stdint.h>
#include <stdbool.h>

void ui_input_window_create();
void ui_input_window_free();
void ui_input_window_resize();
void ui_input_window_set_enabled(bool enabled);
void ui_input_window_try_pop_input(char* out);
void ui_input_window_render();

#endif