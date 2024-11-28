#pragma once

#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

void trmR_in_enable_raw_mode();
void trmR_in_enable_cooked_mode();
int trmR_in_read_input();

#ifdef TRMR_IN_IMPL

#define TRMR_IN_CTRL_KEY(k) ((k) & 0x1f)

enum trmR_in_keys {
    TRMR_IN_ARROW_UP = 1000,
    TRMR_IN_ARROW_DOWN,
    TRMR_IN_ARROW_RIGHT,
    TRMR_IN_ARROW_LEFT,
    TRMR_IN_PGUP,
    TRMR_IN_PGDN,
    TRMR_IN_HOME,
    TRMR_IN_END,
    TRMR_IN_DEL
};

struct termios orig_termios;

void trmR_in_enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw = orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void trmR_in_enable_cooked_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

int trmR_in_read_input() {
    char c;
    read(STDIN_FILENO, &c, 1);
    if(c == '\x1b') {
        char seq[3];
        if(read(STDIN_FILENO, &seq[0], 1) != 1) {
            return '\x1b';
        }
        if(read(STDIN_FILENO, &seq[1], 1) != 1) {
            return '\x1b';
        }
        if(seq[0] == '[') {
            if(read(STDIN_FILENO, &seq[2], 1) != 1) {
                return '\x1b';
            }
            if(seq[2] == '~') {
                switch(seq[1]) {
                    case '1':
                        return TRMR_IN_HOME;
                        break;
                    case '3':
                        return TRMR_IN_DEL;
                        break;
                    case '4':
                        return TRMR_IN_END;
                        break;
                    case '5':
                        return TRMR_IN_PGUP;
                        break;
                    case '6':
                        return TRMR_IN_PGDN;
                        break;
                    case '7':
                        return TRMR_IN_HOME;
                        break;
                    case '8':
                        return TRMR_IN_END;
                        break;
                }
            }
            else {
                switch(seq[1]) {
                    case 'A':
                        return TRMR_IN_ARROW_UP;
                        break;
                    case 'B':
                        return TRMR_IN_ARROW_DOWN;
                        break;
                    case 'C':
                        return TRMR_IN_ARROW_RIGHT;
                        break;
                    case 'D':
                        return TRMR_IN_ARROW_LEFT;
                        break;
                    case 'H':
                        return TRMR_IN_HOME;
                        break;
                    case 'F':
                        return TRMR_IN_END;
                        break;
                }
                return '\x1b';
            }
        }
        else if(seq[0] = 'O') {
            switch(seq[1]) {
                case 'H':
                    return TRMR_IN_HOME;
                    break;
                case 'F':
                    return TRMR_IN_END;
                    break;
            }
        }
    }
    else {
        return c;
    }
}

#endif /* TRMR_IN_IMPL */
