#include "ui/ui.h"
#include "ui/chat_entries.h"
#include "ui/ui_windows/header_window.h"
#include "ui/ui_data.h"

extern UI ui;
static WINDOW* s_header_window;
static char s_server_ip[MAX_IP_BYTES] = "255.255.255.255";
static uint32_t s_client_count = 0;
static bool s_is_connected = false;
static bool s_tls_enabled = false;

void ui_header_window_create()
{
    uint32_t n_cols = getmaxx(stdscr);
    s_header_window = newwin(1, n_cols, 0, 0);
    wbkgd(s_header_window, COLOR_PAIR(COLOR_PAIR_BAR));
}

void ui_header_window_free()
{
    delwin(s_header_window);
}

void ui_header_window_resize()
{
    werase(s_header_window);
    uint32_t n_cols = getmaxx(stdscr);
    wresize(s_header_window, 1, n_cols);
    mvwin(s_header_window, 0, 0);
    ui_header_window_render();
}

void ui_header_set_connected(bool is_connected)
{
    s_is_connected = is_connected;
}

void ui_header_window_set_tls_enabled(bool tls_enabled)
{
    s_tls_enabled = tls_enabled;
}
void ui_header_window_set_connected_clients(uint32_t client_count)
{
    s_client_count = client_count;
}

void ui_header_window_set_server_ip(const char* server_ip)
{
    strcpy(s_server_ip, server_ip);
}

void ui_header_window_render()
{
    pthread_mutex_lock(&ui.render_mutex);

    // Clear the screen before each update
    werase(s_header_window);
    uint32_t char_gap = 2;
    // Centered title
    char title[] = "| THEY CHAT |";
    uint32_t n_cols = getmaxx(s_header_window);
    mvwprintw(s_header_window, 0, (n_cols - strlen(title)) / 2, "%s", title);

    uint32_t col = 0;
    if (s_is_connected) {

        // Connected status
        {
            char connected_status_text[] = "CONNECTED";

            mvwprintw(s_header_window, 0, col, "[");

            col += 1;

            wattron(s_header_window, COLOR_PAIR(COLOR_PAIR_BAR_ENABLED));
            mvwprintw(s_header_window, 0, col, "%s", connected_status_text);
            wattroff(s_header_window, COLOR_PAIR(COLOR_PAIR_BAR_ENABLED));
            col += strlen(connected_status_text);

            mvwprintw(s_header_window, 0, col, "]");
            col += 1;
            col += char_gap;
        }

        // Server IP
        {
            mvwprintw(s_header_window, 0, col, "Server[");
            col += 7;

            wattron(s_header_window, COLOR_PAIR(COLOR_PAIR_BAR_ENABLED));
            mvwprintw(s_header_window, 0, col, "%s", s_server_ip);
            wattroff(s_header_window, COLOR_PAIR(COLOR_PAIR_BAR_ENABLED));
            col += strlen(s_server_ip);

            mvwprintw(s_header_window, 0, col, "]");
            col += 1;
            col += char_gap;
        }
        // Connected user count
        {
            char connected_users[] = "Connected users [";
            mvwprintw(s_header_window, 0, col, "%s", connected_users);
            col += strlen(connected_users);

            wattron(s_header_window, COLOR_PAIR(COLOR_PAIR_BAR_ENABLED));
            char n[3];
            sprintf(n, "%d", s_client_count);
            mvwprintw(s_header_window, 0, col, "%s", n);
            wattroff(s_header_window, COLOR_PAIR(COLOR_PAIR_BAR_ENABLED));
            col += strlen(n);

            mvwprintw(s_header_window, 0, col, "]");
            col += 1;
            col += char_gap;
        }

        // Connected status
        {
            char tls_status[16];
            strcpy(tls_status, s_tls_enabled ? "ENABLED" : "DISABLED");
            uint32_t color_pair = s_tls_enabled ? COLOR_PAIR_BAR_ENABLED : COLOR_PAIR_BAR_DISABLED;

            col = n_cols - strlen(tls_status) - 6;

            mvwprintw(s_header_window, 0, col, "TLS[");

            col += 4;

            wattron(s_header_window, COLOR_PAIR(color_pair));
            mvwprintw(s_header_window, 0, col, "%s", tls_status);
            wattroff(s_header_window, COLOR_PAIR(color_pair));
            col += strlen(tls_status);

            mvwprintw(s_header_window, 0, col, "]");

            col += 1;
            col += char_gap;
        }

    } else {
        // Connected status
        {
            char connected_status_text[] = "DISCONNECTED";
            mvwprintw(s_header_window, 0, col, "[");
            col += 1;

            wattron(s_header_window, COLOR_PAIR(COLOR_PAIR_BAR_DISABLED));
            mvwprintw(s_header_window, 0, col, "%s", connected_status_text);
            wattroff(s_header_window, COLOR_PAIR(COLOR_PAIR_BAR_DISABLED));

            col += strlen(connected_status_text);

            mvwprintw(s_header_window, 0, col, "]");
        }
    }

    wnoutrefresh(s_header_window);
    pthread_mutex_unlock(&ui.render_mutex);
}
