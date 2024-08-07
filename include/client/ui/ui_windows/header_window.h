#ifndef __HEADER_WINDOW_H__
#define __HEADER_WINDOW_H__
#include <stdint.h>
#include <stdbool.h>

void ui_header_window_create();
void ui_header_window_free();
void ui_header_window_resize();
void ui_header_set_connected(bool is_connected);
void ui_header_window_set_tls_enabled(bool tls_enabled);
void ui_header_window_set_connected_clients(uint32_t client_count);
void ui_header_window_set_server_ip(const char* server_ip);
void ui_header_window_render();

#endif