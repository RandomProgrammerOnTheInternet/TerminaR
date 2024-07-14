#pragma once

/*  ----------
 *   TerminaR
 *  ----------
 *  
 *
 *  Notes:
 *  
 *  Coordinates are listed as (y,x) because arrays are weird
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#ifdef TERMINAR_IMPLEMENTATION

#ifndef trmR_set_default_color
    #define trmR_set_default_color trmR_start_color(255, 255, 255)
#endif /* trmR_set_default_color */

#ifndef trmR_cls
    #define trmR_cls system("clear")
#endif /* trmR_cls */

#ifndef trmR_screen_width
    #define trmR_screen_width 256
#endif /* trmR_screen_width */
                            /* make sure these values are even */
#ifndef trmR_screen_height
    #define trmR_screen_height 144
#endif /* trmR_screen_height */

#ifndef trmR_pixel_value
    #define trmR_pixel_value 'm'
#endif /* trmR_pixel */

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef struct {
    u16 x;
    u16 y;
    u8 r;
    u8 g;
    u8 b;
} trmR_pixel_t;

trmR_pixel_t trmR_screen[trmR_screen_height][trmR_screen_width];

void trmR_start_bg_color(u8, u8, u8);
void trmR_start_fg_color(u8, u8, u8);
void trmR_start_color(u8, u8, u8);
void trmR_init_scr(u8, u8, u8);
void trmR_stpx(u16, u16, u8, u8, u8);
void trmR_show_scr();
void trmR_end();
void trmR_lerp_rgb(f32, u8, u8, u8, u8, u8, u8, u8*, u8*, u8*);
void trmR_draw_line_low(u16, u16, u16, u16, u8, u8, u8, u8, u8, u8);
void trmR_draw_line_high(u16, u16, u16, u16, u8, u8, u8, u8, u8, u8);
void trmR_draw_line(u16, u16, u16, u16, u8, u8, u8, u8, u8, u8);

void trmR_start_bg_color(u8 r, u8 g, u8 b) {
    printf("\x1b[48;2;%d;", r);
    printf("%d;", g);
    printf("%dm", b);
}

void trmR_start_fg_color(u8 r, u8 g, u8 b) {
    printf("\x1b[38;2;%d;", r);
    printf("%d;", g);
    printf("%dm", b);
}

void trmR_start_color(u8 r, u8 g, u8 b) {
    trmR_start_bg_color(r, g, b);
    trmR_start_fg_color(r, g, b);
}

void trmR_temp_color(u8 r, u8 g, u8 b) {
    /* todo, might not even do it */
}

void trmR_init_scr(u8 r, u8 g, u8 b) {
    for(int i = 0; i < trmR_screen_height; i++) {
        for(int j = 0; j < trmR_screen_width; j++) {
            trmR_screen[i][j].x = j;
            trmR_screen[i][j].y = i;
            trmR_screen[i][j].r = r; // "jirgb"
            trmR_screen[i][j].g = g;
            trmR_screen[i][j].b = b;
        }
    }
}

void trmR_stpx(u16 y, u16 x, u8 r, u8 g, u8 b) {
    if(y > trmR_screen_height || x > trmR_screen_width) {
        printf("\x1b[38;2;255;255;255m \x1b[48;2;0;0;0m skill issue: you gotta set that pixel INSIDE the screen!\n");
        exit(1);
    }
    trmR_screen[y][x].r = r;
    trmR_screen[y][x].g = g;
    trmR_screen[y][x].b = b;
}

void trmR_show_scr() {
    trmR_cls;
    for(int i = 1; i < trmR_screen_height; i+=2) {
        for(int j = 0; j < trmR_screen_width; j++) {
            trmR_start_bg_color(trmR_screen[i - 1][j].r, trmR_screen[i - 1][j].g, trmR_screen[i - 1][j].b);
            trmR_start_fg_color(trmR_screen[i][j].r, trmR_screen[i][j].g, trmR_screen[i][j].b);
            putchar(trmR_pixel_value);
        }
        printf("\n");
    }
}

void trmR_end() {
    trmR_cls;
    exit(0);
}

void trmR_lerp_rgb(f32 t, u8 r1, u8 g1, u8 b1, u8 r2, u8 g2, u8 b2, u8 *sr, u8 *sg, u8 *sb) {
    *sr = (u8)floorf(((1.0 - t) * (f32)r1) + (t * (f32)r2));
    *sg = (u8)floorf(((1.0 - t) * (f32)g1) + (t * (f32)g2));
    *sb = (u8)floorf(((1.0 - t) * (f32)b1) + (t * (f32)b2));
}

int trmR_draw_line_low(u16 y1, u16 x1, u16 y2, u16 x2, u8 r1, u8 g1, u8 b1, u8 r2, u8 g2, u8 b2) {
    u8 sr;
    u8 sg;
    u8 sb;
    f32 t;
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
        t = (f32)(x - x1) / (f32)pixel_count;
        trmR_lerp_rgb(t, r1, g1, b1, r2, g2, b2, &sr, &sg, &sb);
        trmR_stpx(x, y, sr, sg, sb);
        if(D > 0) {
            y = y + yi;
            D = D + (2 * (dy - dx));
        }
        else {
            D = D + 2 * dy;
        }
    }
    
    return 0;
}

int trmR_draw_line_high(u16 y1, u16 x1, u16 y2, u16 x2, u8 r1, u8 g1, u8 b1, u8 r2, u8 g2, u8 b2) {
    u8 sr;
    u8 sg;
    u8 sb;
    f32 t;
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
        t = (f32)(y - y1) / (f32)pixel_count;
        trmR_lerp_rgb(t, r1, g1, b1, r2, g2, b2, &sr, &sg, &sb);
        trmR_stpx(x, y, sr, sg, sb);
        if(D > 0) {
            x = x + xi;
            D = D + (2 * (dx - dy));
        }
        else {
            D = D + 2 * dx;
        }
    }
    
    return 0;
}

int trmR_draw_line(u16 y1, u16 x1, u16 y2, u16 x2, u8 r1, u8 g1, u8 b1, u8 r2, u8 g2, u8 b2) {
    if(abs(y2 - y1) < abs(x2 - x1)) {
        if(x1 > x2) {
            trmR_draw_line_low(y2, x2, y1, x1, r1, g1, b1, r2, g2, b2);
        }
        else {
            trmR_draw_line_low(y1, x1, y2, x2, r1, g1, b1, r2, g2, b2);
        }
    }
    else {
        if(y1 > y2) {
            trmR_draw_line_high(y2, x2, y1, x1, r1, g1, b1, r2, g2, b2);
        }
        else {
            trmR_draw_line_high(y1, x1, y2, x2, r1, g1, b1, r2, g2, b2);
        }
    }
    
    return 0;
}


#endif /* TERMINAR_IMPLEMENTAION */
