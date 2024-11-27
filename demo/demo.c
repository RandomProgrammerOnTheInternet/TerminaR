#include <stdio.h>
#define TRMR_IMPL
#define trmR_screen_width 100
#define trmR_screen_height 100
#include "../terminar.h"

int main() {
	// initialize screen with RGB color white
	trmR_init_scr(255, 255, 255);
	// set a pixel at (50, 0) with RGB color purple
	trmR_stpx(50, 0, 255, 0, 255);
	// draw a line from (0, 0) to (100, 100) with a gradient from black to red
	trmR_draw_line(0, 0, 100, 100, 0, 0, 0, 255, 0, 0);
	// show the screen
	trmR_show_scr();
	// trmR_end isnt called because we want the image to stay

	return 0;
}
