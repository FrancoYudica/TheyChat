#include "chat_state/ui.h"
#include "chat_state/ui_data.h"
#include "chat_state/ui/log_window.h"

extern UI ui;
static WINDOW* s_log_window;
char s_log[256] = "Welcome to They Chat!.";

void ui_log_window_create()
{
    uint32_t n_rows, n_cols;
    getmaxyx(stdscr, n_rows, n_cols);
    s_log_window = newwin(3, n_cols - 2, n_rows - 6, 1);
}

void ui_log_window_free()
{
    delwin(s_log_window);
}

void ui_log_window_resize()
{
    uint32_t n_rows, n_cols;
    getmaxyx(stdscr, n_rows, n_cols);

    werase(s_log_window);
    wresize(s_log_window, 3, n_cols - 2);
    mvwin(s_log_window, n_rows - 6, 1);

    ui_log_window_render();
}

void ui_log_window_set_log(const char* log)
{
    strcpy(s_log, log);
}

void ui_log_window_render()
{
    pthread_mutex_lock(&ui.render_mutex);

    werase(s_log_window);

    // Add a unique border style or color
    wattron(s_log_window, COLOR_PAIR(ui.soft_color_pair));
    box(s_log_window, 0, 0);

    // Renders log
    {
        uint32_t max_width = getmaxx(s_log_window) - 2; // -2 for the borders
        uint32_t str_len = strlen(s_log);
        uint32_t n = str_len;
        if (n > max_width)
            n = max_width;

        mvwaddnstr(s_log_window, 1, 1, s_log, n);
    }
    wattroff(s_log_window, COLOR_PAIR(ui.soft_color_pair));
    wnoutrefresh(s_log_window);
    pthread_mutex_unlock(&ui.render_mutex);
}
