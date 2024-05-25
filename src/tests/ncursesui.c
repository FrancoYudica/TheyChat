#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

// Constants
#define MAX_MESSAGES 100  // Maximum number of messages
#define MESSAGE_LENGTH 256  // Maximum length of each message

// Global variables
char messages[MAX_MESSAGES][MESSAGE_LENGTH];  // Array to store messages
int message_count = 0;  // Total number of messages
int current_message = 0;  // Index of the current message being displayed

WINDOW *chat_win;  // Window to display chat messages
WINDOW *input_win;  // Window to input messages

// Mutex for thread synchronization
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Function declarations
void init_windows();
void destroy_windows();
void add_message(const char *message);
void *receive_messages(void *arg);
void draw_chat();
void scroll_up();
void scroll_down();
void handle_resize();

int main() {
    return 0;
    pthread_t receiver_thread;  // Thread to simulate receiving messages
    char input[MESSAGE_LENGTH];  // Buffer to store user input
    int ch;  // Variable to store keyboard input

    // Initialize ncurses
    initscr();
    cbreak();  // Disable line buffering
    noecho();  // Disable echoing of typed characters
    keypad(stdscr, TRUE);  // Enable reading of function keys
    signal(SIGWINCH, handle_resize);  // Handle window resize

    // Initialize windows
    init_windows();

    // Create a thread to simulate receiving messages
    pthread_create(&receiver_thread, NULL, receive_messages, NULL);

    const char type_hint[] = "Type a message: ";

    // // Main loop for input
    // while (1) {
    //     // Clear input window and display prompt
    //     wclear(input_win);
    //     box(input_win, 0, 0);
    //     mvwprintw(input_win, 1, 1, type_hint);
    //     wrefresh(input_win);

    //     echo();  // Enable echoing for input
    //     mvwgetnstr(input_win, 1, sizeof(type_hint), input, sizeof(input) - 1);
    //     noecho();  // Disable echoing after input
    //     if (input[0] == KEY_UP) {
    //         // Scroll up when the up arrow key is pressed
    //         scroll_up();
    //     } else if (input[0] == KEY_DOWN) {
    //         // Scroll down when the down arrow key is pressed
    //         scroll_down();
    //     } else {

    //         // Exit on "quit"
    //         if (strcmp(input, "quit") == 0) {
    //             break;
    //         }

    //         // Add message to chat window
    //         add_message(input);
    //         // Move cursor to input position and refresh input window to show typed characters
    //         wmove(input_win, 1, sizeof(type_hint) + strlen(input));
    //         wrefresh(input_win);
    //     }
    // }

    // Main loop for input
    while (1) {
        // Clear input window and display prompt
        wclear(input_win);
        box(input_win, 0, 0);
        mvwprintw(input_win, 1, 1, type_hint);
        wrefresh(input_win);

        echo();  // Enable echoing for input
        mvwgetnstr(input_win, 1, sizeof(type_hint), input, sizeof(input) - 1);
        noecho();  // Disable echoing after input
        if (input[strlen(input) - 1] == KEY_UP) {
            // Scroll up when the up arrow key is pressed
            scroll_up();
        } else if (input[strlen(input) - 1] == KEY_DOWN) {
            // Scroll down when the down arrow key is pressed
            scroll_down();
        } else {

            // Exit on "quit"
            if (strcmp(input, "quit") == 0) {
                break;
            }

            // Add message to chat window
            add_message(input);
            wrefresh(input_win);
        }
    }

    // Cleanup
    pthread_cancel(receiver_thread);
    pthread_join(receiver_thread, NULL);
    destroy_windows();
    endwin();

    return 0;
}

// Initialize the windows
void init_windows() {
    int height, width;
    getmaxyx(stdscr, height, width);  // Get the size of the terminal window

    // Create the chat window and input window
    chat_win = newwin(height - 3, width, 0, 0);
    input_win = newwin(3, width, height - 3, 0);

    // Enable keypad mode for windows
    keypad(chat_win, TRUE);
    keypad(input_win, TRUE);

    scrollok(chat_win, TRUE);  // Enable scrolling in the chat window

    // Draw boxes around the windows
    box(chat_win, 0, 0);
    box(input_win, 0, 0);
    wrefresh(chat_win);
    wrefresh(input_win);
}

// Destroy the windows
void destroy_windows() {
    delwin(chat_win);
    delwin(input_win);
}

// Add a new message to the chat
void add_message(const char *message) {
    pthread_mutex_lock(&mutex);  // Lock the mutex
    if (message_count < MAX_MESSAGES) {
        strncpy(messages[message_count], message, MESSAGE_LENGTH);  // Copy the message to the array
        message_count++;
        // Update current_message to keep the latest messages visible
        if (message_count > current_message + getmaxy(chat_win) - 2) {
            current_message = message_count - getmaxy(chat_win) + 2;
        }
    }
    pthread_mutex_unlock(&mutex);  // Unlock the mutex
    draw_chat();  // Redraw the chat window
}

// Draw the chat window
void draw_chat() {
    pthread_mutex_lock(&mutex);  // Lock the mutex
    wclear(chat_win);  // Clear the chat window
    box(chat_win, 0, 0);
    int start = (message_count > MAX_MESSAGES) ? message_count - MAX_MESSAGES : 0;
    int lines = getmaxy(chat_win) - 2;  // Number of lines available for messages
    for (int i = current_message; i < message_count && i - current_message < lines; i++) {
        mvwprintw(chat_win, i - current_message + 1, 1, "%s", messages[i]);
    }
    wrefresh(chat_win);  // Refresh the chat window
    pthread_mutex_unlock(&mutex);  // Unlock the mutex
}

// Scroll the chat window up
void scroll_up() {
    pthread_mutex_lock(&mutex);  // Lock the mutex
    if (current_message > 0) {
        current_message--;
        draw_chat();  // Redraw the chat window
    }
    pthread_mutex_unlock(&mutex);  // Unlock the mutex
}

// Scroll the chat window down
void scroll_down() {
    pthread_mutex_lock(&mutex);  // Lock the mutex
    if (current_message < message_count - getmaxy(chat_win) + 2) {
        current_message++;
        draw_chat();  // Redraw the chat window
    }
    pthread_mutex_unlock(&mutex);  // Unlock the mutex
}

// Thread function to simulate receiving messages
void *receive_messages(void *arg) {
    char buffer[MESSAGE_LENGTH];
    int counter = 0;
    
    while (1) {
        // Simulate receiving a message every 5 seconds
        sleep(5);
        snprintf(buffer, MESSAGE_LENGTH, "Received message %d", ++counter);
        add_message(buffer);
    }

    return NULL;
}

// Handle window resize
void handle_resize(int sig) {
    endwin();  // End the current window
    refresh();  // Refresh the screen to handle resize
    clear();  // Clear the screen

    // Reinitialize the windows
    init_windows();

    // Redraw the chat window content
    draw_chat();
}
