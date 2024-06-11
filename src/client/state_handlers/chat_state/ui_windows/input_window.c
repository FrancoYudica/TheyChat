#include "chat_state/ui.h"
#include <ctype.h>
void render_input_window(UI* ui, char* input)
{
    // Keeps track of the length of the last input
    static size_t last_input_length = 0;

    // Stores the last input text
    static char last_input_text[MAX_CHAT_TEXT_BYTES] = { 0 };

    pthread_mutex_lock(&ui->render_mutex);

    // Make input non-blocking. This is very important, otherwise
    // the input thread will keep the mutex, and chat wont be able
    // to update
    nodelay(ui->input_window, TRUE);

    bool want_to_refresh = false;

    if (ui->input_enabled) {
        // Make the cursor visible when input is enabled
        curs_set(1);
        char prompt_message[] = "Type message: ";

        // Display the prompt message
        mvwprintw(ui->input_window, 1, 1, "%s", prompt_message);

        // Move the cursor to the end of the input text, where the next char will be placed
        wmove(ui->input_window, 1, sizeof(prompt_message) + strlen(ui->input_text));

        // Get a character from the input window
        int ch = wgetch(ui->input_window);

        if (ch != ERR) {
            size_t input_length = strlen(ui->input_text);

            // Handle the enter key (send message)
            if (ch == '\n') {

                // Copies into output buffer
                memcpy(input, ui->input_text, input_length + 1);

                // Clear the input text
                memset(ui->input_text, 0, sizeof(ui->input_text));

            } else if ((ch == KEY_BACKSPACE || ch == 127 || ch == '\b') && input_length > 0) {
                // Handle backspace, remove last character
                ui->input_text[input_length - 1] = '\0';
            } else if (ch == KEY_UP) {

                if (ui->chat_entries_offset > 0) {
                    ui->chat_entries_offset--;
                    want_to_refresh = true;
                }
            } else if (ch == KEY_DOWN) {

                ui->chat_entries_offset++;
                want_to_refresh = true;

            } else if (isprint(ch) && input_length < sizeof(ui->input_text) - 1) {
                // Append the typed character to the input text
                ui->input_text[input_length] = ch;
                ui->input_text[input_length + 1] = '\0';
            }

            // Update the display only if the input text has changed
            if (strcmp(last_input_text, ui->input_text) != 0) {
                // Display the input text
                mvwprintw(ui->input_window, 1, sizeof(prompt_message), "%s", ui->input_text);

                // Clear the rest of the line
                wclrtoeol(ui->input_window);

                // Update the last input text
                strncpy(last_input_text, ui->input_text, sizeof(last_input_text) - 1);
                last_input_text[sizeof(last_input_text) - 1] = '\0';
            }
        }
        curs_set(0);
    } else {

        // Set the color pair for disabled input
        wattron(ui->input_window, COLOR_PAIR(2));
        char prompt_message[] = "Input disabled";
        mvwprintw(ui->input_window, 1, 1, "%s", prompt_message);

        // Turn off the color pair
        wattroff(ui->input_window, COLOR_PAIR(2));

        // Wait for a character to be entered
        wgetch(ui->input_window);
    }

    wrefresh(ui->input_window);
    pthread_mutex_unlock(&ui->render_mutex);

    if (want_to_refresh)
        ui_refresh(ui);
}
