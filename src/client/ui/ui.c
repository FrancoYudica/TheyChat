#include <sys/ioctl.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdarg.h>

#include "ui/ui.h"
#include "ui/input_handler.h"
#include "ui/chat_entries.h"
#include "ui/ui_data.h"
#include "ui/ui_windows/header_window.h"
#include "ui/ui_windows/chat_window.h"
#include "ui/ui_windows/log_window.h"
#include "ui/ui_windows/input_window.h"

UI ui;

// ID for colors
enum Colors {
    BAR_BG_COLOR = 8,
    BAR_ENABLED_COLOR,
    BAR_DISABLED_COLOR,
    CHAT_BG_COLOR,
    CHAT_USERNAME_COLOR,
    CHAT_NOTIFICATION_COLOR,
    CHAT_ALTERNATIVE_COLOR
};

static void initialize_color_pairs()
{
    use_default_colors();
    // Sets colors values
    init_color(BAR_BG_COLOR, 329, 321, 500);
    init_color(BAR_ENABLED_COLOR, 0, 1000, 0);
    init_color(BAR_DISABLED_COLOR, 1000, 0, 0);

    init_color(CHAT_BG_COLOR, 100, 100, 150);
    init_color(CHAT_USERNAME_COLOR, 0, 1000, 600);
    init_color(CHAT_NOTIFICATION_COLOR, 1000, 600, 0);
    init_color(CHAT_ALTERNATIVE_COLOR, 400, 400, 700);

    // Initializes color pairs, with corresponding backgrounds
    init_pair(COLOR_PAIR_BAR, COLOR_WHITE, BAR_BG_COLOR);
    init_pair(COLOR_PAIR_BAR_ENABLED, BAR_ENABLED_COLOR, BAR_BG_COLOR);
    init_pair(COLOR_PAIR_BAR_DISABLED, BAR_DISABLED_COLOR, BAR_BG_COLOR);

    init_pair(COLOR_PAIR_CHAT, COLOR_WHITE, CHAT_BG_COLOR);
    init_pair(COLOR_PAIR_CHAT_USERNAME, CHAT_USERNAME_COLOR, CHAT_BG_COLOR);
    init_pair(COLOR_PAIR_CHAT_NOTIFICATION, CHAT_NOTIFICATION_COLOR, CHAT_BG_COLOR);
    init_pair(COLOR_PAIR_CHAT_ALTERNATIVE, CHAT_ALTERNATIVE_COLOR, CHAT_BG_COLOR);
}

static void window_resize()
{
    pthread_mutex_lock(&ui.render_mutex);
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    erase();
    resizeterm(w.ws_row, w.ws_col);
    refresh();
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

    memset(&ui, 0, sizeof(UI));
    pthread_mutex_init(&ui.render_mutex, NULL);

    // Sets up basic colors
    if (has_colors()) {
        start_color();
        initialize_color_pairs();
    }

    ui_header_window_create();
    ui_chat_window_create();
    ui_log_window_create();
    ui_input_window_create();

    // Enable function keys (F1, F2, arrow keys, etc.)
    keypad(stdscr, TRUE);

    // Window resize signal handler
    signal(SIGWINCH, window_resize);

    // Initializes input handler, a new thread to receive input
    input_handler_init();
}

void ui_set_connected(bool is_connected)
{
    ui_header_set_connected(is_connected);
}

void ui_set_tls_enabled(bool tls_enabled)
{
    ui_header_window_set_tls_enabled(tls_enabled);
}
void ui_add_chat_entry(ChatEntry entry)
{
    pthread_mutex_lock(&ui.render_mutex);
    ui_chat_window_add_entry(entry);
    pthread_mutex_unlock(&ui.render_mutex);

    ui_chat_window_render();
}

void ui_set_log_text(const char* format, ...)
{
    pthread_mutex_lock(&ui.render_mutex);
    char buffer[512];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    ui_log_window_set_log(buffer);
    pthread_mutex_unlock(&ui.render_mutex);

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

void ui_set_input_prompt(const char* text)
{
    ui_input_window_set_prompt(text);
}

void ui_push_text_entry(enum TextEntryType type, const char* format, ...)
{
    char buffer[1024]; // Adjust the size as needed
    va_list args;

    // Start the varargs processing
    va_start(args, format);

    // Format the string into the buffer
    vsnprintf(buffer, sizeof(buffer), format, args);

    // End the varargs processing
    va_end(args);

    // Create and populate the ChatEntry
    ChatEntry entry;
    entry.type = CHAT_ENTRY_SERVER_NOTIFICATION;
    strncpy(entry.data.text.text, buffer, sizeof(entry.data.text.text));
    chat_entry_format_time(&entry, time(NULL));
    ui_add_chat_entry(entry);
}

void ui_refresh()
{
    ui_header_window_render();
    ui_chat_window_render();
    ui_log_window_render();
    ui_input_window_render();
    doupdate();
}
void ui_free()
{
    ui_header_window_free();
    ui_chat_window_free();
    ui_log_window_free();
    ui_input_window_free();
    endwin();
}
