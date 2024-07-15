#include "ui/ui_data.h"

#ifdef __unix__
#include <sys/ioctl.h>

#elif defined(_WIN32)
#include <windows.h>
#endif

extern UI ui;

void terminal_resize()
{

#ifdef __unix__
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    erase();
    resizeterm(w.ws_row, w.ws_col);
    refresh();

#elif defined(_WIN32) || defined(_WIN64)

    // Get console screen buffer info
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        fprintf(stderr, "GetConsoleScreenBufferInfo failed\n");
        pthread_mutex_unlock(&ui.render_mutex);
        return;
    }

    // Extract rows and columns from the screen buffer info
    int rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    int cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    erase();
    resizeterm(rows, cols);
    refresh();
#endif
}