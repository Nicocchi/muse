/*** includes ***/
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/*** defines ***/

#define CTRL_KEY(k) ((k) & 0x1f)

/*** data ***/

struct termios orig_termios;

/*** terminal ***/

// Prints an error message and exits the program
void die(const char *s) {
    perror(s);
    exit(1);
}

// Disable raw mode
void disableRawMode() {
    // Store the terminal attributes to the struct and apply it to the terminal when
    // the program exits
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

// Enable raw mode
void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
    // Read the current attributes into a struct
    tcgetattr(STDIN_FILENO, &orig_termios);

    // Register disableRawMode() function to be called automatically when the program exits
    atexit(disableRawMode);

    struct termios raw = orig_termios;

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

/*** init ***/

int main() {
    enableRawMode();

    while (1) {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
        if (iscntrl(c)) {
            printf("%d\r\n", c);
        } 
        else {
            printf("%d ('%c')\r\n", c, c);
        }
        if (c == CTRL_KEY('q')) break;
    }

    return 0;
}
