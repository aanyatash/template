#include "fb.h"

typedef unsigned int color_t;

static const color_t WHITE = 0xffffffff;
static const color_t RED = 0xffff0000;
static const color_t YELLOW = 0xffffff00;
static const int WIDTH = 1024;
static const int HEIGHT = 768;
static const int DEPTH = 4;

void draw_pixel(int x, int y, color_t c)
{
    unsigned int per_row = fb_get_pitch()/DEPTH; // length of each row in pixels (include any padding)
    unsigned int (*im)[per_row] = fb_get_draw_buffer();
    im[y][x] = c;
}

void draw_hline(int y, color_t c)
{
    for( int x = 0; x < fb_get_width(); x++ )
        draw_pixel(x, y, c);
}

void draw_vline(int x, color_t c)
{
    for( int y = 0; y < fb_get_height(); y++ )
        draw_pixel(x, y, c);
}

void draw_rectangle(int x, int y, int length, int width, color_t c)
{
    for( int i = 0; i < length; i ++ ) {
        for( int j = 0; j < width; j ++) {
            draw_pixel(i + x, j + y, c);
		}
    }
}


void main(void) 
{
    fb_init(WIDTH, HEIGHT, DEPTH, FB_SINGLEBUFFER);

//    for( int y = 0; y < HEIGHT; y += 16 )
//        draw_hline(y, RED);
//
//    for( int x = 0; x < WIDTH; x += 16 )
//        draw_vline(x, YELLOW);

   for (int x = 0; x < WIDTH; x += 32) {
       for (int y = 0; y < HEIGHT; y += 32) {
   		   draw_rectangle(x, y, 16, 16, WHITE); 
		}
   }

   for (int x = 16; x < WIDTH; x += 32) {
       for (int y = 16; y < HEIGHT; y += 32) {
   		   draw_rectangle(x, y, 16, 16, WHITE); 
		}
   }


}

