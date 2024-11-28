#include <stdio.h>
#define TRMR_IMPL
#define trmR_screen_width 10
#define trmR_screen_height 10
#include "../terminar.h"
#define TRMR_IN_IMPL
#include "../terminar_input.h"

int main() {
    trmR_init_scr(255, 255, 255);
    trmR_in_enable_raw_mode();
    int x = 0;
    int y = 0;
    while(1) {
        switch(trmR_in_read_input()) {
            case 'w':
            case 'W':
                trmR_stpx(x, y, 255, 255, 255);
                y--;
                break;
            case 'a':
            case 'A':
                trmR_stpx(x, y, 255, 255, 255);
                x--;
                break;
            case 's':
            case 'S':
                trmR_stpx(x, y, 255, 255, 255);
                y++;
                break;
            case 'd':
            case 'D':
                trmR_stpx(x, y, 255, 255, 255);
                x++;
                break;
            case 'q':
            case 'Q':
                break;
        }
        trmR_stpx(x, y, 0, 0, 0);
        trmR_show_scr();
    }
    trmR_in_enable_cooked_mode();
    trmR_end();

    return 0;
}
