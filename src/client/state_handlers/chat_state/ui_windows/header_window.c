#include "chat_state/ui.h"

void render_header_window(UI* ui)
{
    pthread_mutex_lock(&ui->render_mutex);

    // Clear the screen before each update
    wclear(ui->header_window);

    // Centered title
    char title[] = "| THEY CHAT |";
    mvwprintw(ui->header_window, 0, (COLS - strlen(title)) / 2, "%s", title);

    // Connected user count
    char connected_users[] = "Connected users: ";

    uint32_t col = COLS - strlen(connected_users) - 3;
    wattron(ui->header_window, COLOR_PAIR(ui->soft_color_pair));
    mvwprintw(ui->header_window, 0, col, "%s", connected_users);
    wattroff(ui->header_window, COLOR_PAIR(ui->soft_color_pair));

    col += strlen(connected_users);

    wattron(ui->header_window, COLOR_PAIR(ui->name_color_pair));
    mvwprintw(ui->header_window, 0, col, "%d", ui->connected_count);
    wattroff(ui->header_window, COLOR_PAIR(ui->name_color_pair));

    // Server IP
    wattron(ui->header_window, COLOR_PAIR(ui->soft_color_pair));
    mvwprintw(ui->header_window, 0, 1, "Server (%s)", ui->server_ip);
    wattroff(ui->header_window, COLOR_PAIR(ui->soft_color_pair));

    wrefresh(ui->header_window);
    pthread_mutex_unlock(&ui->render_mutex);
}