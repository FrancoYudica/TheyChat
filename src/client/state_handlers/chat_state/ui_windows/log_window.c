#include "chat_state/ui.h"

void render_log_window(UI* ui)
{
    pthread_mutex_lock(&ui->render_mutex);

    wclear(ui->log_window);

    // Add a unique border style or color
    wattron(ui->log_window, COLOR_PAIR(ui->soft_color_pair));
    box(ui->log_window, 0, 0);
    mvwprintw(ui->log_window, 1, 1, "Log: %s", ui->log);
    wattroff(ui->log_window, COLOR_PAIR(ui->soft_color_pair));
    wrefresh(ui->log_window);
    pthread_mutex_unlock(&ui->render_mutex);
}