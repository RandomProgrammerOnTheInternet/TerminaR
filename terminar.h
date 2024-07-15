/*  ----------
 *   TerminaR
 *  ----------
 *  STB style terminal rendering library.
 *
 * This file provides both the interface and the implementation.
 * To init the implementation,
 *      #define TERMINAR_IMPL
 * in *one* source file, before #including to generate the implementation.
 *
 * Examples are in examples folder.
 *
 * History:
 *      0.02 (therealblue24) refactor
 *      0.01 first public release
 *
 * NOTES:
 *  - therealblue24 saved us from the wrath of the (y, x) array coordinates. 
 *    no more worries!
 *  - there is no alpha channel, so transparency is your job to manage
 *  - This is a pretty bare bones library, which gives you low level access
 *    to the screen and the ability to set and get pixels. You can also
 *    draw lines and thats about it.
 *  - If you are planning to use this for a serious project, either:
 *      A) Optimize rendering to use a flash buffer and only change
 *         pixels when needed
 *      or,
 *      B) If the project requires >= 7 FPS, don't use TerminaR.
*/

#ifndef TERMINAR_H_
#define TERMINAR_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#ifndef trmR_set_default_color
#define trmR_set_default_color trmR_start_color(255, 255, 255)
#endif /* trmR_set_default_color */

/* 
 * this doesn't actually clear the terminal, but rather it goes to cell (0, 0) 
 * of the terminal to continue drawing
 */
#ifndef trmR_cls
#define trmR_cls() printf("\033[H")
#endif /* trmR_cls */

#ifndef trmR_screen_width
#define trmR_screen_width 256
#endif /* trmR_screen_width */

/* height needs to be even */
#ifndef trmR_screen_height
#define trmR_screen_height 144
#endif /* trmR_screen_height */

#ifndef trmR_pixel_value
#define trmR_pixel_value "▄"
#endif /* trmR_pixel */

#ifndef trmR_min
#define trmR_min(a, b)      \
    ({                      \
        typeof(a) _a = (a); \
        typeof(b) _b = (b); \
        _a < _b ? _a : _b;  \
    })
#endif /* trmR_min */

#ifndef trmR_max
#define trmR_max(a, b)      \
    ({                      \
        typeof(a) _a = (a); \
        typeof(b) _b = (b); \
        _a > _b ? _a : _b;  \
    })
#endif /* trmR_max */

#ifndef trmR_clamp
#define trmR_clamp(val, lo, hi) (trmR_min((hi), trmR_max((lo), (val))))
#endif /* trmR_clamp */

/* setting a pixel out of bounds should just clamp, not error */
#ifndef trmR_stpx
#define trmR_stpx(x, y, _c)                                     \
    do {                                                        \
        trmR_screen[trmR_clamp(x, 0, trmR_screen_width - 1) +   \
                    (trmR_screen_width *                        \
                     trmR_clamp(y, 0, trmR_screen_height - 1))] \
            .c = _c;                                            \
    } while(0);
#endif /* trmR_stpx */

#ifndef trmR_stpxrgb
#define trmR_stpxrgb(x, y, _r, _g, _b) \
    trmR_stpx(x, y, ((trmR_pixel_t){ .r = _r, .g = _g, .b = _b }).c)
#endif /* trmR_stpxrgb */

#ifndef trmR_gtpx
#define trmR_gtpx(x, y)                                    \
    (trmR_screen[trmR_clamp(x, 0, trmR_screen_width - 1) + \
                 (trmR_screen_width *                      \
                  trmR_clamp(y, 0, trmR_screen_height - 1))])
#endif /* trmR_gtpx */

typedef union {
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };
    uint32_t c;
} trmR_pixel_t;
extern trmR_pixel_t *trmR_screen;

void trmR_start_bg_color(uint8_t, uint8_t, uint8_t);
void trmR_start_fg_color(uint8_t, uint8_t, uint8_t);
void trmR_start_color(uint8_t, uint8_t, uint8_t);
void trmR_init_scr(uint8_t, uint8_t, uint8_t);
void trmR_show_scr(void);
void trmR_end(void);
void trmR_quickend(void);
void trmR_lerp_rgb(float, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t,
                   uint8_t *, uint8_t *, uint8_t *);
int trmR_draw_line(uint16_t, uint16_t, uint16_t, uint16_t, uint8_t, uint8_t,
                   uint8_t, uint8_t, uint8_t, uint8_t);

#endif /* TERMINAR_H_ */

#ifdef __CLANGD__
#define TERMINAR_IMPL
#endif

#ifdef TERMINAR_IMPL

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

trmR_pixel_t *trmR_screen = NULL;

/* Since randomprogrammerontheinternet decided to only use the create, and optionally
 * destroy model we have to do these shenatigans. */
__attribute__((destructor(0))) void __trmrcleanup(void)
{
    /* todo: find a better solution */
    if(trmR_screen && (void *)(free)) {
        free(trmR_screen);
        trmR_screen = NULL;
    }
}

static int trmR_draw_line_low(uint16_t, uint16_t, uint16_t, uint16_t, uint8_t,
                              uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
static int trmR_draw_line_high(uint16_t, uint16_t, uint16_t, uint16_t, uint8_t,
                               uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);

void trmR_start_bg_color(uint8_t r, uint8_t g, uint8_t b)
{
    printf("\x1b[48;2;%d;", r);
    printf("%d;", g);
    printf("%dm", b);
}

void trmR_start_fg_color(uint8_t r, uint8_t g, uint8_t b)
{
    printf("\x1b[38;2;%d;", r);
    printf("%d;", g);
    printf("%dm", b);
}

void trmR_start_color(uint8_t r, uint8_t g, uint8_t b)
{
    trmR_start_bg_color(r, g, b);
    trmR_start_fg_color(r, g, b);
}

void trmR_temp_color(uint8_t r, uint8_t g, uint8_t b)
{
    /* todo, might not even do it */
}

void trmR_init_scr(uint8_t r, uint8_t g, uint8_t b)
{
    /* TODO: multiple channel (other than 3) support */
    trmR_screen = calloc(trmR_screen_height * 4, trmR_screen_width);
    for(int i = 0; i < trmR_screen_height; i++) {
        for(int j = 0; j < trmR_screen_width; j++) {
            trmR_stpxrgb(j, i, r, g, b);
        }
    }
}

void trmR_show_scr(void)
{
    trmR_cls();
    for(int i = 1; i < trmR_screen_height; i += 2) {
        for(int j = 0; j < trmR_screen_width; j++) {
            trmR_pixel_t a = trmR_gtpx(j, i - 1);
            trmR_pixel_t b = trmR_gtpx(j, i);
            trmR_start_bg_color(a.r, a.g, a.b);
            trmR_start_fg_color(b.r, b.g, b.b);
            printf(trmR_pixel_value);
        }
        printf("\n");
    }
    fflush(stdout);
}

__attribute__((noreturn)) void trmR_end(void)
{
    trmR_cls();
    if(trmR_screen) {
        free(trmR_screen);
        trmR_screen = NULL;
    }
    exit(0);
}

__attribute__((noreturn)) void trmR_quickend(void)
{
    if(trmR_screen) {
        free(trmR_screen);
        trmR_screen = NULL;
    }
    exit(0);
}

void trmR_lerp_rgb(float t, uint8_t r1, uint8_t g1, uint8_t b1, uint8_t r2,
                   uint8_t g2, uint8_t b2, uint8_t *sr, uint8_t *sg,
                   uint8_t *sb)
{
    *sr = (uint8_t)floorf(((1.0 - t) * (float)r1) + (t * (float)r2));
    *sg = (uint8_t)floorf(((1.0 - t) * (float)g1) + (t * (float)g2));
    *sb = (uint8_t)floorf(((1.0 - t) * (float)b1) + (t * (float)b2));
}

int trmR_draw_line_low(uint16_t y1, uint16_t x1, uint16_t y2, uint16_t x2,
                       uint8_t r1, uint8_t g1, uint8_t b1, uint8_t r2,
                       uint8_t g2, uint8_t b2)
{
    uint8_t sr;
    uint8_t sg;
    uint8_t sb;
    float t;
    int pixel_count = abs(x2 - x1);
    int dx = x2 - x1;
    int dy = y2 - y1;
    int yi = 1;
    if(dy < 0) {
        yi = -1;
        dy = -dy;
    }
    int D = (2 * dy) - dx;

    int y = y1;

    for(int x = x1; x < x2; x++) {
        t = (float)(x - x1) / (float)pixel_count;
        trmR_lerp_rgb(t, r1, g1, b1, r2, g2, b2, &sr, &sg, &sb);
        trmR_stpxrgb(x, y, sr, sg, sb);
        if(D > 0) {
            y = y + yi;
            D = D + (2 * (dy - dx));
        } else {
            D = D + 2 * dy;
        }
    }

    return 0;
}

int trmR_draw_line_high(uint16_t y1, uint16_t x1, uint16_t y2, uint16_t x2,
                        uint8_t r1, uint8_t g1, uint8_t b1, uint8_t r2,
                        uint8_t g2, uint8_t b2)
{
    uint8_t sr;
    uint8_t sg;
    uint8_t sb;
    float t;
    int pixel_count = abs(y2 - y1);
    int dx = x2 - x1;
    int dy = y2 - y1;
    int xi = 1;
    if(dx < 0) {
        xi = -1;
        dx = -dx;
    }
    int D = (2 * dx) - dy;
    int x = x1;

    for(int y = y1; y < y2; y++) {
        t = (float)(y - y1) / (float)pixel_count;
        trmR_lerp_rgb(t, r1, g1, b1, r2, g2, b2, &sr, &sg, &sb);
        trmR_stpxrgb(x, y, sr, sg, sb);
        if(D > 0) {
            x = x + xi;
            D = D + (2 * (dx - dy));
        } else {
            D = D + 2 * dx;
        }
    }

    return 0;
}

int trmR_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
                   uint8_t r1, uint8_t g1, uint8_t b1, uint8_t r2, uint8_t g2,
                   uint8_t b2)
{
    if(abs(y2 - y1) < abs(x2 - x1)) {
        if(x1 > x2) {
            trmR_draw_line_low(y2, x2, y1, x1, r1, g1, b1, r2, g2, b2);
        } else {
            trmR_draw_line_low(y1, x1, y2, x2, r1, g1, b1, r2, g2, b2);
        }
    } else {
        if(y1 > y2) {
            trmR_draw_line_high(y2, x2, y1, x1, r1, g1, b1, r2, g2, b2);
        } else {
            trmR_draw_line_high(y1, x1, y2, x2, r1, g1, b1, r2, g2, b2);
        }
    }

    return 0;
}

#endif /* TERMINAR_IMPL */
