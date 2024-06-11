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
    char connected[64];
    sprintf(connected, "Connected users: %d", ui->connected_count);
    mvwprintw(ui->header_window, 0, COLS - strlen(connected) - 1, "%s", connected);

    // Server IP
    mvwprintw(ui->header_window, 0, 1, "Server (%s)", ui->server_ip);

    wrefresh(ui->header_window);
    pthread_mutex_unlock(&ui->render_mutex);
}