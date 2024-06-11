#include "chat_state/ui.h"
#include "chat_state/chat_entries.h"
extern void render_header_window(UI* ui);
extern void render_chat_window(UI* ui);
extern void render_log_window(UI* ui);

static WINDOW* create_newwin(int rows, int columns, int start_row, int start_column)
{
    WINDOW* win = newwin(rows, columns, start_row, start_column);
    box(win, 0, 0);
    return win;
}

static void initialize_color_pairs(UI* ui)
{
    ui->white_color_pair = 1;
    ui->soft_color_pair = 2;
    // Soft color
    init_color(8, 400, 400, 400);
    init_pair(ui->white_color_pair, COLOR_GREEN, COLOR_BLACK);
    init_pair(ui->soft_color_pair, 8, COLOR_BLACK);

    ui->chat_color_pair_min = 9;
    ui->chat_color_pair_max = ui->chat_color_pair_min + 9;
    uint16_t colors[10][3] = {
        { 1000, 0, 0 }, // Red
        { 0, 1000, 0 }, // Green
        { 0, 0, 1000 }, // Blue
        { 1000, 1000, 0 }, // Yellow
        { 1000, 0, 1000 }, // Magenta
        { 0, 1000, 1000 }, // Cyan
        { 1000, 500, 0 }, // Orange
        { 0, 1000, 500 }, // Lime
        { 500, 0, 1000 }, // Purple
        { 500, 1000, 0 }, // Chartreuse
    };

    for (uint8_t i = 0; i < 10; i++) {
        uint32_t color = ui->chat_color_pair_min + i;
        init_color(color, colors[i][0], colors[i][1], colors[i][2]);
        init_pair(color, color, COLOR_BLACK);
    }
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

    ui->header_window = create_newwin(1, COLS, 0, 0);
    ui->chat_window = create_newwin(LINES - 7, COLS, 1, 0);
    ui->input_window = create_newwin(3, COLS, LINES - 3, 0);

    uint32_t chat_bottom_row = getmaxy(ui->chat_window);
    ui->log_window = create_newwin(3, COLS - 2, chat_bottom_row + 1, 1);

    // Sets up basic colors
    start_color();
    initialize_color_pairs(ui);

    // Enable function keys (F1, F2, arrow keys, etc.)
    keypad(stdscr, TRUE);
    keypad(ui->input_window, TRUE);
}

void ui_add_chat_entry(UI* ui, const char* name, const char* text)
{
    pthread_mutex_lock(&ui->render_mutex);
    chat_entries_add(ui->chat_entries, name, text);
    pthread_mutex_unlock(&ui->render_mutex);

    render_chat_window(ui);
}

void ui_refresh(UI* ui)
{
    render_header_window(ui);
    render_chat_window(ui);
    render_log_window(ui);
}
void ui_free(UI* ui)
{
    delwin(ui->header_window);
    delwin(ui->chat_window);
    delwin(ui->log_window);
    delwin(ui->input_window);
    chat_entries_free(ui->chat_entries);
    endwin();
}
