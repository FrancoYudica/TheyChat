#include "chat_state/ui.h"
#include "chat_state/chat_entries.h"
#include "signal.h"
#include "chat_state/ui_data.h"
#include "chat_state/ui/header_window.h"
#include "chat_state/ui/chat_window.h"
#include "chat_state/ui/log_window.h"
#include "chat_state/ui/input_window.h"
#include <sys/ioctl.h>
#include <unistd.h>
UI ui;

static void initialize_color_pairs()
{

    uint8_t SOFT_COLOR = 8;

    init_color(SOFT_COLOR, 700, 700, 700);

    ui.white_color_pair = 1;
    ui.soft_color_pair = 2;
    ui.name_color_pair = 3;

    // Soft color
    init_pair(ui.white_color_pair, COLOR_GREEN, COLOR_BLACK);
    init_pair(ui.soft_color_pair, SOFT_COLOR, COLOR_BLACK);
    init_pair(ui.name_color_pair, COLOR_MAGENTA, COLOR_BLACK);
}

static void window_resize()
{
    pthread_mutex_lock(&ui.render_mutex);
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    erase();
    resizeterm(w.ws_row, w.ws_col);
    refresh(); // Refresh the screen to apply changes
    pthread_mutex_unlock(&ui.render_mutex);

    ui_header_window_resize();
    ui_chat_window_resize();
    ui_log_window_resize();
    ui_input_window_resize();
}

void ui_init()
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

    memset(&ui, 0, sizeof(UI));
    pthread_mutex_init(&ui.render_mutex, NULL);
    ui_header_window_create();
    ui_chat_window_create();
    ui_log_window_create();
    ui_input_window_create();

    // Sets up basic colors
    start_color();
    initialize_color_pairs(ui);

    // Enable function keys (F1, F2, arrow keys, etc.)
    keypad(stdscr, TRUE);

    // Window resize signal handler
    signal(SIGWINCH, window_resize);

    // window_resize();
}

void ui_add_chat_entry(ChatEntry entry)
{
    pthread_mutex_lock(&ui.render_mutex);
    ui_chat_window_add_entry(entry);
    pthread_mutex_unlock(&ui.render_mutex);

    ui_chat_window_render();
}

void ui_set_log_text(const char* text)
{
    ui_log_window_set_log(text);
    ui_log_window_render();
}

void ui_set_connected_count(uint32_t count)
{
    ui_header_window_set_connected_clients(count);
}

void ui_set_server_ip(const char* ip)
{
    ui_header_window_set_server_ip(ip);
}

void ui_try_pop_input(char* output_buffer)
{
    ui_input_window_render();
    ui_input_window_try_pop_input(output_buffer);
}

void ui_refresh()
{
    ui_header_window_render();
    ui_chat_window_render();
    ui_log_window_render();
}
void ui_free()
{
    ui_header_window_free();
    ui_chat_window_free();
    ui_log_window_free();
    ui_input_window_free();
    endwin();
}
