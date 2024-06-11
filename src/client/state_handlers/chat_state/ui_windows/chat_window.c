#include "chat_state/ui.h"
#include "chat_state/chat_entries.h"

void render_chat_window(UI* ui)
{
    pthread_mutex_lock(&ui->render_mutex);

    wclear(ui->chat_window);
    uint32_t n_columns, n_rows;
    getmaxyx(ui->chat_window, n_rows, n_columns);

    // Renders outline box
    wattron(ui->chat_window, COLOR_PAIR(ui->white_color_pair));
    box(ui->chat_window, 0, 0);
    wattroff(ui->chat_window, COLOR_PAIR(ui->white_color_pair));

    // Subtracts top and bottom box outline characters
    uint32_t bottom_row = n_rows - 2;

    ChatEntriesIterator* iterator = chat_entries_create_iterator(ui->chat_entries, ui->chat_entries_offset);

    uint32_t index = 0;

    while (chat_entries_iterator_has_more(iterator)) {
        uint32_t moved_row = 1 + index++;

        if (moved_row > bottom_row)
            break;

        const ChatEntry* entry;
        chat_entries_iterator_get_current(iterator, &entry);

        // Keeps track of column
        uint32_t col = 1;

        // Renders time
        wattron(ui->chat_window, COLOR_PAIR(ui->soft_color_pair));
        mvwprintw(ui->chat_window, moved_row, col, " %02d:%02d ", entry->hour, entry->minute);
        wattroff(ui->chat_window, COLOR_PAIR(ui->soft_color_pair));
        col += 8;

        // Renders name
        wattron(ui->chat_window, COLOR_PAIR(ui->name_color_pair));
        mvwprintw(ui->chat_window, moved_row, col, "%s", entry->name);
        wattroff(ui->chat_window, COLOR_PAIR(ui->name_color_pair));
        col += strlen(entry->name);

        // Renders IP
        wattron(ui->chat_window, COLOR_PAIR(ui->soft_color_pair));
        mvwprintw(ui->chat_window, moved_row, col, " (%s)", entry->ip);
        wattroff(ui->chat_window, COLOR_PAIR(ui->soft_color_pair));
        col += strlen(entry->ip) + 3;

        // Renders text
        mvwprintw(ui->chat_window, moved_row, col, ": %s", entry->text);

        chat_entries_iterator_move_next(iterator);
    }

    chat_entries_free_iterator(iterator);

    // Refreshes after rendering
    wrefresh(ui->chat_window);
    pthread_mutex_unlock(&ui->render_mutex);
}