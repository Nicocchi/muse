/*** includes ***/
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

/*** defines ***/

#define CTRL_KEY(k) ((k) & 0x1f)

/*** data ***/

struct editorConfig {
    int screenrows;
    int screencols;
    struct termios orig_termios;
};

struct editorConfig E;

/*** terminal ***/

// Prints an error message and exits the program
void die(const char *s) {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    perror(s);
    exit(1);
}

// Disable raw mode
void disableRawMode() {
    // Store the terminal attributes to the struct and apply it to the terminal when
    // the program exits
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
        die("tcsetattr");
}

// Enable raw mode
void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) die("tcgetattr");
    // Read the current attributes into a struct

    // Register disableRawMode() function to be called automatically when the program exits
    atexit(disableRawMode);

    struct termios raw = E.orig_termios;

    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_iflag &= ~(ICRNL | IXON); // Disable ctrl-s and ctrl-q
    raw.c_oflag &= ~(OPOST); // Turn off output processing
    raw.c_cflag |= (CS8);

    // Modify the struct and turn off echo and canonical mode
    // Read the input byte-byte not line-by-line
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0; // Return as soon as there is any input to be read
    raw.c_cc[VTIME] = 1; // Set the maximum amount of time to wait before read() returns

    // Write the terminal attributes back out
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcgetattr");
}

// Wait for one keypress and return it
char editorReadKey() {
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN) die("read");
    }
    return c;
}

// Get the window size
int getWindowSize(int *rows, int*cols) {
    struct winsize ws;

    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        return -1;
    }
    else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

/*** output ***/

// Draw each row of the buffer of text being edited
void editorDrawRows() {
    int y;
    for (y = 0; y < E.screenrows; y++) {
        write(STDIN_FILENO, "~\r\n", 3); // Writes the ~
    }
}

// Clear the screen
void editorRefreshScreen() {
    write(STDOUT_FILENO, "\x1b[2J", 4); // Writes an escape sequence to the terminal
    write(STDOUT_FILENO, "\x1b[H", 3); // Reposition the cursor
    
    editorDrawRows();

    write(STDOUT_FILENO, "\x1b[H", 3);
}

/*** input ***/

// Wait for a keypress and then handle it
void editorProcessKeypress() {
    char c = editorReadKey();

    switch(c) {
        case CTRL_KEY('q'):
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);
            break;
    }
}

/*** init ***/

void initEditor() {
    if (getWindowSize(&E.screenrows, &E.screencols) == -1) die("getWindowSize");
}

int main() {
    enableRawMode();
    initEditor();

    while (1) {
        editorRefreshScreen();
        editorProcessKeypress();
    }

    return 0;
}
