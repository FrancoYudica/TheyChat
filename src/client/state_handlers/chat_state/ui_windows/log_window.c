#include "chat_state/ui.h"

void render_log_window(UI* ui)
{
    pthread_mutex_lock(&ui->render_mutex);

    wclear(ui->log_window);

    // Add a unique border style or color
    wattron(ui->log_window, COLOR_PAIR(ui->soft_color_pair));
    box(ui->log_window, 0, 0);

    // Renders log
    {
        uint32_t max_width = getmaxx(ui->log_window) - 2; // -2 for the borders
        uint32_t str_len = strlen(ui->log);
        uint32_t n = str_len;
        if (n > max_width)
            n = max_width;

        mvwaddnstr(ui->log_window, 1, 1, ui->log, n);
    }
    wattroff(ui->log_window, COLOR_PAIR(ui->soft_color_pair));
    wrefresh(ui->log_window);
    pthread_mutex_unlock(&ui->render_mutex);
}