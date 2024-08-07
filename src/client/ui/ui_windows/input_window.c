#include <ctype.h>
#include <stdbool.h>
#include "ui/ui.h"
#include "ui/chat_entries.h"
#include "ui/ui_windows/chat_window.h"
#include "ui/ui_windows/input_window.h"
#include "ui/ui_data.h"

extern UI ui;
static WINDOW* s_input_window;
static bool s_input_enabled = true;
static char s_input_text[MAX_CHAT_TEXT_BYTES];
static char s_submitted_text[MAX_CHAT_TEXT_BYTES];
static char s_prompt_message[128] = "Type message:";
static char s_disabled_prompt[] = "Input disabled";

void ui_input_window_create()
{
    uint32_t n_rows, n_cols;
    getmaxyx(stdscr, n_rows, n_cols);
    s_input_window = newwin(1, n_cols, n_rows - 1, 0);
    wbkgd(s_input_window, COLOR_PAIR(COLOR_PAIR_CHAT));
    keypad(s_input_window, TRUE);

    memset(s_input_text, 0, sizeof(s_input_text));
    memset(s_submitted_text, 0, sizeof(s_submitted_text));
}

void ui_input_window_free()
{
    delwin(s_input_window);
}

void ui_input_window_resize()
{
    uint32_t n_rows, n_cols;
    getmaxyx(stdscr, n_rows, n_cols);
    werase(s_input_window);
    wresize(s_input_window, 1, n_cols);
    mvwin(s_input_window, n_rows - 1, 0);
    ui_input_window_render();
}
void ui_input_window_set_enabled(bool enabled)
{
    s_input_enabled = enabled;
}

void ui_input_window_set_prompt(const char* text)
{
    strcpy(s_prompt_message, text);

    // Clears previous prompt message
    pthread_mutex_lock(&ui.render_mutex);
    wmove(s_input_window, 0, 0);
    wclrtoeol(s_input_window);
    pthread_mutex_unlock(&ui.render_mutex);
}
void ui_input_window_try_pop_input(char* out)
{
    if (s_submitted_text[0] != '\0') {
        strcpy(out, s_submitted_text);
        s_submitted_text[0] = '\0';
    }
}

void ui_input_window_render()
{
    // Keeps track of the length of the last input
    static size_t last_input_length = 0;

    // Stores the last input text
    static char last_input_text[MAX_CHAT_TEXT_BYTES] = { 0 };

    pthread_mutex_lock(&ui.render_mutex);

    // Make input non-blocking. This is very important, otherwise
    // the input thread will keep the mutex, and chat won't be able
    // to update
    nodelay(s_input_window, TRUE);

    bool want_to_scroll = false;

    if (s_input_enabled) {
        // Make the cursor visible when input is enabled
        curs_set(1);
        int prompt_length = strlen(s_prompt_message);

        // Calculate the maximum width for the input text
        int max_width = getmaxx(s_input_window);
        int max_input_length = max_width - prompt_length - 1;

        // Display the prompt message
        wattron(s_input_window, COLOR_PAIR(COLOR_PAIR_CHAT_ALTERNATIVE));
        mvwprintw(s_input_window, 0, 0, "%s", s_prompt_message);
        wattroff(s_input_window, COLOR_PAIR(COLOR_PAIR_CHAT_ALTERNATIVE));
        size_t input_length = strlen(s_input_text);

        // Move the cursor to the end of the input text, where the next char will be placed
        if (1 + input_length > max_input_length) {
            wmove(s_input_window, 0, max_width - 1);
        } else
            wmove(s_input_window, 0, prompt_length + input_length + 1);

        // Get a character from the input window
        int ch = wgetch(s_input_window);

        if (ch != ERR) {

            // Handle the enter key (send message)
            if (ch == '\n') {
                // Copies into output buffer
                memcpy(s_submitted_text, s_input_text, input_length + 1);

                // Clear the input text
                memset(s_input_text, 0, sizeof(s_input_text));

            } else if (ch == KEY_BACKSPACE && input_length > 0) {
                // Handle backspace, remove last character
                s_input_text[input_length - 1] = '\0';
            } else if (ch == KEY_UP) {
                ui_chat_window_scroll_up();
                want_to_scroll = true;
            } else if (ch == KEY_DOWN) {
                ui_chat_window_scroll_down();
                want_to_scroll = true;
            } else if (isprint(ch) && input_length < sizeof(s_input_text) - 1) {
                // Append the typed character to the input text
                s_input_text[input_length] = ch;
                s_input_text[++input_length] = '\0';
            }

            // Update the display only if the input text has changed
            if (strcmp(last_input_text, s_input_text) != 0) {
                const char* start_str = s_input_text;

                // Only renders a segment of the input in case all space is occupied is reached.
                if (input_length + 1 > max_input_length)
                    start_str += 1 + input_length - max_input_length;

                // Display the input text
                mvwprintw(s_input_window, 0, prompt_length + 1, "%s", start_str);
                // Clear the rest of the line
                wclrtoeol(s_input_window);

                // Update the last input text
                last_input_text[sizeof(last_input_text) - 1] = '\0';
                strncpy(last_input_text, s_input_text, sizeof(last_input_text) - 1);
            }
        }
    } else {
        curs_set(0);
        // Set the color pair for disabled input
        mvwprintw(s_input_window, 0, 0, "%s", s_disabled_prompt);
        wgetch(s_input_window);
    }

    wnoutrefresh(s_input_window);
    pthread_mutex_unlock(&ui.render_mutex);

    // If it scrolls, renders the chat window
    if (want_to_scroll)
        ui_refresh();
}
