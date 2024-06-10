#include "chat_state/ui.h"
#include "chat_state/chat_entries.h"

void render_chat_window(UI* ui)
{
    wclear(ui->chat_window);
    uint32_t n_columns, n_rows;
    getmaxyx(ui->chat_window, n_rows, n_columns);

    // Renders outline box
    box(ui->chat_window, 0, 0);

    // Subtracts top and bottom box outline characters
    uint32_t bottom_row = n_rows - 2;

    ChatEntriesIterator* iterator = chat_entries_create_iterator(ui->chat_entries, ui->chat_entries_offset);

    uint32_t index = 0;
    wattron(ui->chat_window, COLOR_PAIR(1)); // Turn on color pair 1

    while (chat_entries_iterator_has_more(iterator)) {
        uint32_t moved_row = 1 + index++;

        if (moved_row > bottom_row)
            break;

        const char* name;
        const char* text;
        chat_entries_iterator_get_current(iterator, &name, &text);

        // Prints name and text
        mvwprintw(ui->chat_window, moved_row, 1, "%s: %s", name, text);
        chat_entries_iterator_move_next(iterator);
    }
    wattroff(ui->chat_window, COLOR_PAIR(1)); // Turn on color pair 1

    chat_entries_free_iterator(iterator);

    // Refreshes after rendering
    wrefresh(ui->chat_window);
}