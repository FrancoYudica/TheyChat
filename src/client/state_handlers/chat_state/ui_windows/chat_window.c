#include "chat_state/ui.h"
#include "chat_state/chat_entries.h"
#include "chat_state/ui/chat_window.h"
#include "chat_state/ui_data.h"

extern UI ui;

static WINDOW* s_chat_window;

// Chat window data
ChatEntries* chat_entries;
uint32_t chat_entries_offset;

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
    const char* src_text)
{
    uint32_t n_columns, n_rows;
    getmaxyx(win, n_rows, n_columns);

    uint32_t max_column = n_columns - 2;
    uint32_t max_row = n_rows - 2;
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
    s_chat_window = newwin(rows - 7, cols, 1, 0);

    chat_entries = chat_entries_create();
    chat_entries_offset = 0;
}

void ui_chat_window_free()
{
    delwin(s_chat_window);
    chat_entries_free(chat_entries);
}

void ui_chat_window_resize()
{
    uint32_t rows, cols;
    getmaxyx(stdscr, rows, cols);

    werase(s_chat_window);
    wresize(s_chat_window, rows - 7, cols);
    mvwin(s_chat_window, 1, 0);
    ui_chat_window_render();
}

// Functions used to render the different ChatEntry types
static void render_user_text_entry(const ChatEntry* entry, uint32_t* row, uint32_t col);
static void render_server_notification_entry(const ChatEntry* entry, uint32_t* row, uint32_t col);

void ui_chat_window_render()
{
    pthread_mutex_lock(&ui.render_mutex);

    werase(s_chat_window);
    uint32_t n_columns, n_rows;
    getmaxyx(s_chat_window, n_rows, n_columns);

    // Renders outline box
    wattron(s_chat_window, COLOR_PAIR(ui.chat_box_color_pair));
    box(s_chat_window, 0, 0);
    wattroff(s_chat_window, COLOR_PAIR(ui.chat_box_color_pair));

    // Subtracts top and bottom box outline characters
    uint32_t bottom_row = n_rows - 2;

    ChatEntriesIterator* iterator = chat_entries_create_iterator(chat_entries, chat_entries_offset);

    uint32_t row = 0;

    while (chat_entries_iterator_has_more(iterator)) {
        row++;

        if (row > bottom_row)
            break;

        const ChatEntry* entry;
        chat_entries_iterator_get_current(iterator, &entry);

        // Keeps track of column
        uint32_t col = 1;

        // Renders time
        wattron(s_chat_window, COLOR_PAIR(ui.soft_color_pair));
        mvwprintw(s_chat_window, row, col, "%03d %02d:%02d ", row + chat_entries_offset, entry->time.hour, entry->time.minute);
        wattroff(s_chat_window, COLOR_PAIR(ui.soft_color_pair));
        col += 11;

        switch (entry->type) {
        case CHAT_ENTRY_USER_TEXT:
            render_user_text_entry(entry, &row, col);
            break;

        case CHAT_ENTRY_SERVER_NOTIFICATION:
            render_server_notification_entry(entry, &row, col);
            break;
        default:
            break;
        }

        chat_entries_iterator_move_next(iterator);
    }

    chat_entries_free_iterator(iterator);

    // Refreshes after rendering
    wnoutrefresh(s_chat_window);
    pthread_mutex_unlock(&ui.render_mutex);
}

void ui_chat_window_add_entry(ChatEntry chat_entry)
{
    chat_entries_add(chat_entries, chat_entry);
}
void ui_chat_window_scroll_up()
{
    if (chat_entries_offset > 0)
        chat_entries_offset--;
}
void ui_chat_window_scroll_down()
{
    chat_entries_offset++;
}
static void render_user_text_entry(const ChatEntry* entry, uint32_t* row, uint32_t col)
{

    const UserTextChatEntry* user_text = &entry->data.user_text;

    // Renders name
    wattron(s_chat_window, COLOR_PAIR(ui.name_color_pair));
    mvwprintw(s_chat_window, *row, col, "%s", user_text->name);
    wattroff(s_chat_window, COLOR_PAIR(ui.name_color_pair));
    col += strlen(user_text->name);

    // Renders IP
    wattron(s_chat_window, COLOR_PAIR(ui.soft_color_pair));
    mvwprintw(s_chat_window, *row, col, " (%s): ", user_text->ip);
    wattroff(s_chat_window, COLOR_PAIR(ui.soft_color_pair));
    col += strlen(user_text->ip) + 5;

    // Renders text
    mvwprint_multiline(s_chat_window, row, col, user_text->text);
}

static void render_server_notification_entry(const ChatEntry* entry, uint32_t* row, uint32_t col)
{
    const ServerNotificationPayloadChatEntry* server_notification = &entry->data.server_notification;
    mvwprint_multiline(s_chat_window, row, col, server_notification->text);
}