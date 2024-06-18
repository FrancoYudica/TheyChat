#include "chat_state/ui.h"
#include "chat_state/chat_entries.h"
#include "signal.h"

extern void render_header_window(UI* ui);
extern void render_chat_window(UI* ui);
extern void render_log_window(UI* ui);

static UI* s_ui;

static void initialize_color_pairs(UI* ui)
{

    uint8_t SOFT_COLOR = 8;

    init_color(SOFT_COLOR, 700, 700, 700);

    ui->white_color_pair = 1;
    ui->soft_color_pair = 2;
    ui->name_color_pair = 3;

    // Soft color
    init_pair(ui->white_color_pair, COLOR_GREEN, COLOR_BLACK);
    init_pair(ui->soft_color_pair, SOFT_COLOR, COLOR_BLACK);
    init_pair(ui->name_color_pair, COLOR_MAGENTA, COLOR_BLACK);
}

// static void window_resize()
// {
//     uint32_t n_rows, n_cols;
//     getmaxyx(stdscr, n_rows, n_cols);
//     clear();
//     // Adjust header window
//     wresize(s_ui->header_window, 1, n_cols);
//     mvwin(s_ui->header_window, 0, 0);

//     // Adjust chat window
//     wresize(s_ui->chat_window, n_rows - 7, n_cols);
//     mvwin(s_ui->chat_window, 1, 0);

//     // Move input window to the bottom
//     mvwin(s_ui->input_window, n_rows - 3, 0);
//     wresize(s_ui->input_window, 3, n_cols);

//     // Adjust log window below chat window
//     uint32_t chat_bottom_row = getmaxy(s_ui->chat_window);
//     mvwin(s_ui->log_window, chat_bottom_row + 1, 1);
//     wresize(s_ui->log_window, 3, n_cols - 2);

//     // Refresh all windows to apply changes
//     ui_refresh(s_ui);
// }

static void window_resize()
{
    uint32_t n_rows, n_cols;
    getmaxyx(stdscr, n_rows, n_cols);
    clear(); // Clear the screen before resizing

    // Adjust header window
    wresize(s_ui->header_window, 1, n_cols);
    mvwin(s_ui->header_window, 0, 0); // Move to top-left corner

    // Adjust chat window
    wresize(s_ui->chat_window, n_rows - 7, n_cols);
    mvwin(s_ui->chat_window, 1, 0); // Move just below header

    // Move input window to the bottom
    mvwin(s_ui->input_window, n_rows - 3, 0);
    wresize(s_ui->input_window, 3, n_cols);

    // Adjust log window below chat window
    uint32_t chat_bottom_row = getmaxy(s_ui->chat_window) + getbegy(s_ui->chat_window);
    mvwin(s_ui->log_window, chat_bottom_row + 1, 1); // Position just below chat window
    wresize(s_ui->log_window, 3, n_cols - 2); // Adjust width accordingly

    ui_refresh(s_ui);
    refresh(); // Refresh the screen to apply changes
}

void ui_init(UI* ui)
{
    initscr(); // Initialize the window
    cbreak(); // Line buffering disabled
    noecho(); // Don't echo while we do getch
    curs_set(0); // Hides the cursor
    if (has_colors() == FALSE) {
        endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }
    s_ui = ui;

    memset(ui, 0, sizeof(UI));

    // Initializes data structure and windows
    ui->chat_entries = chat_entries_create();
    ui->chat_entries_offset = 0;
    memset(ui->log, 0, sizeof(ui->log));
    char default_log[] = "Welcome to They Chat, go ahead and start sending messages!";
    memcpy(ui->log, default_log, sizeof(default_log));

    char default_ip[] = "0.0.0.0";
    memcpy(ui->server_ip, default_ip, sizeof(default_ip));
    ui->connected_count = 0;

    ui->input_enabled = true;

    memset(ui->input_text, 0, sizeof(ui->input_text));

    pthread_mutex_init(&ui->render_mutex, NULL);

    ui->header_window = newwin(1, COLS, 0, 0);
    ui->chat_window = newwin(LINES - 7, COLS, 1, 0);
    ui->input_window = newwin(3, COLS, LINES - 3, 0);

    uint32_t chat_bottom_row = getmaxy(ui->chat_window);
    ui->log_window = newwin(3, COLS - 2, chat_bottom_row + 1, 1);

    // Sets up basic colors
    start_color();
    initialize_color_pairs(ui);

    // Enable function keys (F1, F2, arrow keys, etc.)
    keypad(stdscr, TRUE);
    keypad(ui->input_window, TRUE);

    // Window resize signal handler
    signal(SIGWINCH, window_resize);

    window_resize();
}

void ui_add_chat_entry(UI* ui, ChatEntry entry)
{
    pthread_mutex_lock(&ui->render_mutex);
    chat_entries_add(ui->chat_entries, entry);
    pthread_mutex_unlock(&ui->render_mutex);

    render_chat_window(ui);
}

void ui_set_log_text(UI* ui, const char* text)
{
    strcpy(ui->log, text);
    render_log_window(ui);
}
void ui_refresh(UI* ui)
{
    render_header_window(ui);
    render_chat_window(ui);
    render_log_window(ui);
}
void ui_free(UI* ui)
{
    delwin(s_ui->header_window);
    delwin(s_ui->chat_window);
    delwin(s_ui->log_window);
    delwin(s_ui->input_window);
    chat_entries_free(ui->chat_entries);
    endwin();
}
