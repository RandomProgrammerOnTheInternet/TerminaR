#include <stdio.h>
#define TERMINAR_IMPL
#define trmR_screen_width 100
#define trmR_screen_height 100
// NOTE: the ../ is because the demo is not in the root dir
#include "../terminar.h"

int main()
{
    // initialize screen with RGB color white
    trmR_init_scr(255, 255, 255);
    // set a pixel at (25, 0) with RGB color purple, note how it is ordered (x, y)
    trmR_stpxrgb(25, 0, 255, 0, 255);
    // draw a line from (5, 10) to (60, 80) with a gradient from black to red
    trmR_draw_line(5, 10, 60, 80, 0, 0, 0, 255, 0, 0);
    // show the screen
    trmR_show_scr();
    // trmR_end isnt called because we want the image to stay,
    // however we can call trmR_quickend to keep the image
    trmR_quickend();

    return 0;
}
