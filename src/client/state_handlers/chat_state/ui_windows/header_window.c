#include "chat_state/ui.h"
#include "chat_state/ui_data.h"
#include "chat_state/ui/header_window.h"

extern UI ui;

static WINDOW* s_header_window;

static char s_server_ip[MAX_IP_BYTES] = "255.255.255.255";

static uint32_t s_client_count = 0;

void ui_header_window_create()
{
    uint32_t n_cols = getmaxx(stdscr);
    s_header_window = newwin(1, n_cols, 0, 0);
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
    wclear(s_header_window);

    // Centered title
    char title[] = "| THEY CHAT |";
    uint32_t n_cols = getmaxx(s_header_window);
    mvwprintw(s_header_window, 0, (n_cols - strlen(title)) / 2, "%s", title);

    // Connected user count
    char connected_users[] = "Connected users: ";

    uint32_t col = n_cols - strlen(connected_users) - 3;
    wattron(s_header_window, COLOR_PAIR(ui.soft_color_pair));
    mvwprintw(s_header_window, 0, col, "%s", connected_users);
    wattroff(s_header_window, COLOR_PAIR(ui.soft_color_pair));

    col += strlen(connected_users);

    wattron(s_header_window, COLOR_PAIR(ui.name_color_pair));
    mvwprintw(s_header_window, 0, col, "%d", s_client_count);
    wattroff(s_header_window, COLOR_PAIR(ui.name_color_pair));

    // Server IP
    wattron(s_header_window, COLOR_PAIR(ui.soft_color_pair));
    mvwprintw(s_header_window, 0, 1, "Server (%s)", s_server_ip);
    wattroff(s_header_window, COLOR_PAIR(ui.soft_color_pair));

    wrefresh(s_header_window);
    pthread_mutex_unlock(&ui.render_mutex);
}
