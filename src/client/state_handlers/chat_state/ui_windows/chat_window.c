#include "chat_state/ui.h"
#include "chat_state/chat_entries.h"

uint32_t hash_string_to_n(const char* str, uint32_t min_n, uint32_t max_n)
{
    uint64_t acc;

    for (uint32_t i = 0; i < strlen(str); i++) {
        acc += str[i] * (1 + i);
    }
    return min_n + acc % (1 + max_n - min_n);
}

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

        const char* name;
        const char* text;
        chat_entries_iterator_get_current(iterator, &name, &text);

        // Prints name and text
        // uint32_t color_pair_id = hash_string_to_n(name, ui->chat_color_pair_min, ui->chat_color_pair_max);
        // wattron(ui->chat_window, COLOR_PAIR(color_pair_id));
        mvwprintw(ui->chat_window, moved_row, 1, "%s: %s", name, text);
        // wattroff(ui->chat_window, COLOR_PAIR(color_pair_id));

        chat_entries_iterator_move_next(iterator);
    }

    chat_entries_free_iterator(iterator);

    // Refreshes after rendering
    wrefresh(ui->chat_window);
    pthread_mutex_unlock(&ui->render_mutex);
}