#include "ui/ui.h"
#include "ui/chat_entries.h"
#include "ui/ui_windows/log_window.h"
#include "ui/ui_data.h"

extern UI ui;
static WINDOW* s_log_window;
static char s_log[256] = "Welcome to They Chat!.";

void ui_log_window_create()
{
    uint32_t n_rows, n_cols;
    getmaxyx(stdscr, n_rows, n_cols);
    s_log_window = newwin(1, n_cols, n_rows - 2, 0);
    wbkgd(s_log_window, COLOR_PAIR(COLOR_PAIR_BAR));
    werase(s_log_window);
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
    wresize(s_log_window, 1, n_cols);
    mvwin(s_log_window, n_rows - 2, 0);

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

    // Renders log
    {
        uint32_t max_width = getmaxx(s_log_window);
        uint32_t str_len = strlen(s_log);
        uint32_t n = str_len;
        if (n > max_width)
            n = max_width;

        mvwprintw(s_log_window, 0, 0, "%s", s_log);
    }
    wnoutrefresh(s_log_window);
    pthread_mutex_unlock(&ui.render_mutex);
}
