#include "chat_state/ui.h"

void render_log_window(UI* ui)
{
    wclear(ui->log_window);

    // Add a unique border style or color
    wattron(ui->log_window, COLOR_PAIR(2));
    box(ui->log_window, 0, 0);
    mvwprintw(ui->log_window, 1, 1, "Log: %s", ui->log);
    wattroff(ui->log_window, COLOR_PAIR(2));
    wrefresh(ui->log_window);
}