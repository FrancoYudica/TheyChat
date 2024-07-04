#include "ui/ui.h"
#include "ui/chat_entries.h"
#include "ui/ui_windows/chat_window.h"
#include "ui/ui_data.h"

extern UI ui;

static WINDOW* s_chat_window;

// Chat window data
static ChatEntries* s_chat_entries;
static uint32_t s_chat_entries_offset;

static uint32_t min(uint32_t a, uint32_t b)
{
    return (a > b) ? b : a;
}

/// @brief Renders multiline text, checking that the text is always inside the window
/// @param row pointer to the row, since this function moves the row for next text rendering
/// @param src_column fixed colum where text always starts, even in new rows.
static void mvwprint_multiline(
    WINDOW* win,
    uint32_t* row,
    uint32_t src_column,
    uint32_t max_row,
    uint32_t max_column,
    const char* src_text)
{
    uint32_t text_len = strlen(src_text);
    uint32_t text_space = max_column - src_column;

    const char* text_ptr = src_text;

    // Text will be rendered overflows the window
    uint32_t remaining_text_len = text_len;
    while (remaining_text_len != 0 && max_row >= *row) {

        // Amount of characters that will get rendered
        uint32_t render_chars = min(strlen(text_ptr), text_space);

        // Renders a section of the string
        mvwaddnstr(win, *row, src_column, text_ptr, render_chars);

        // Offsets text pointer
        text_ptr += render_chars;
        remaining_text_len -= render_chars;

        // Offsets to next row if there are characters left
        if (remaining_text_len > 0)
            (*row) += 1;
    }
}

void ui_chat_window_create()
{
    uint32_t rows, cols;
    getmaxyx(stdscr, rows, cols);
    s_chat_window = newwin(rows - 3, cols, 1, 0);
    wbkgd(s_chat_window, COLOR_PAIR(COLOR_PAIR_CHAT));
    s_chat_entries = chat_entries_create();
    s_chat_entries_offset = 0;
}

void ui_chat_window_free()
{
    delwin(s_chat_window);
    chat_entries_free(s_chat_entries);
}

void ui_chat_window_resize()
{
    uint32_t rows, cols;
    getmaxyx(stdscr, rows, cols);
    werase(s_chat_window);
    wresize(s_chat_window, rows - 3, cols);
    mvwin(s_chat_window, 1, 0);
    ui_chat_window_render();
}

// Functions used to render the different ChatEntry types
static void render_user_text_entry(const ChatEntry* entry, uint32_t* row, uint32_t col);
static void render_text_entry(const ChatEntry* entry, uint32_t* row, uint32_t col);

void ui_chat_window_render()
{
    pthread_mutex_lock(&ui.render_mutex);

    werase(s_chat_window);
    uint32_t n_columns, n_rows;
    getmaxyx(s_chat_window, n_rows, n_columns);

    // Subtracts top and bottom box outline characters
    uint32_t bottom_row = n_rows;

    ChatEntriesIterator* iterator = chat_entries_create_iterator(s_chat_entries, s_chat_entries_offset);

    uint32_t row = 0;

    // Buffer that stores the formatted message prefix, containing date and time
    char prefix_buffer[100];

    while (chat_entries_iterator_has_more(iterator)) {

        if (row > bottom_row)
            break;

        const ChatEntry* entry;
        chat_entries_iterator_get_current(iterator, &entry);

        // Keeps track of column
        uint32_t col = 0;

        // Renders time
        wattron(s_chat_window, COLOR_PAIR(COLOR_PAIR_CHAT_ALTERNATIVE));
        sprintf(prefix_buffer, "%d %s - ", row + s_chat_entries_offset, entry->time_str);
        mvwprintw(s_chat_window, row, col, "%s", prefix_buffer);
        wattroff(s_chat_window, COLOR_PAIR(COLOR_PAIR_CHAT_ALTERNATIVE));

        col += strlen(prefix_buffer);

        switch (entry->type) {
        case CHAT_ENTRY_USER_TEXT:
            render_user_text_entry(entry, &row, col);
            break;

        case CHAT_ENTRY_TEXT:
            render_text_entry(entry, &row, col);
            break;
        default:
            break;
        }

        chat_entries_iterator_move_next(iterator);
        row++;
    }

    chat_entries_free_iterator(iterator);

    // Refreshes after rendering
    wnoutrefresh(s_chat_window);
    pthread_mutex_unlock(&ui.render_mutex);
}

void ui_chat_window_add_entry(ChatEntry chat_entry)
{
    chat_entries_add(s_chat_entries, chat_entry);
}
void ui_chat_window_scroll_up()
{
    if (s_chat_entries_offset > 0)
        s_chat_entries_offset--;
}
void ui_chat_window_scroll_down()
{
    s_chat_entries_offset++;
}
static void render_user_text_entry(
    const ChatEntry* entry,
    uint32_t* row,
    uint32_t col)
{

    const UserTextChatEntry* user_text = &entry->data.user_text;

    // Renders name
    wattron(s_chat_window, COLOR_PAIR(COLOR_PAIR_CHAT_USERNAME));
    mvwprintw(s_chat_window, *row, col, "%s", user_text->name);
    wattroff(s_chat_window, COLOR_PAIR(COLOR_PAIR_CHAT_USERNAME));
    col += strlen(user_text->name);

    wattron(s_chat_window, COLOR_PAIR(COLOR_PAIR_CHAT_ALTERNATIVE));
    mvwprintw(s_chat_window, *row, col, ": ");
    wattroff(s_chat_window, COLOR_PAIR(COLOR_PAIR_CHAT_ALTERNATIVE));
    col += 2;

    // Renders text
    uint32_t n_columns, n_rows;
    getmaxyx(s_chat_window, n_rows, n_columns);
    uint32_t max_column = n_columns;
    uint32_t max_row = n_rows;
    wattron(s_chat_window, COLOR_PAIR(COLOR_PAIR_CHAT_TEXT));
    mvwprint_multiline(s_chat_window, row, col, max_row, max_column, user_text->text);
    wattroff(s_chat_window, COLOR_PAIR(COLOR_PAIR_CHAT_TEXT));
}

static void render_text_entry(
    const ChatEntry* entry,
    uint32_t* row,
    uint32_t col)
{
    const TextChatEntry* text = &entry->data.text;

    // Gets color pair
    uint8_t color_pair = 0;
    switch (text->text_type) {
    case TEXT_ENTRY_TYPE_LOG:
        color_pair = COLOR_PAIR_CHAT_TEXT;
        break;
    case TEXT_ENTRY_TYPE_SERVER:
        color_pair = COLOR_PAIR_CHAT_SERVER;
        break;
    case TEXT_ENTRY_TYPE_WARNING:
        color_pair = COLOR_PAIR_CHAT_WARNING;
        break;
    default:
        break;
    }

    // Gets display boundaries
    uint32_t n_columns, n_rows;
    getmaxyx(s_chat_window, n_rows, n_columns);
    uint32_t max_column = n_columns;
    uint32_t max_row = n_rows;

    // Renders text
    wattron(s_chat_window, COLOR_PAIR(color_pair));
    mvwprint_multiline(s_chat_window, row, col, max_row, max_column, text->text);
    wattroff(s_chat_window, COLOR_PAIR(color_pair));
}