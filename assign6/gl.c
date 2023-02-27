#include "gl.h"



// use pitch not width, no bytes per pxoxel

void gl_init(unsigned int width, unsigned int height, gl_mode_t mode)
{
    fb_init(width, height, 4, mode);    // use 32-bit depth always for graphics library
}

void gl_swap_buffer(void)
{
    // TODO: implement this function
}

unsigned int gl_get_width(void)
{
    return fb_get_pitch()/fb_get_depth();
}

unsigned int gl_get_height(void)
{
    return fb_get_height();
}

color_t gl_color(unsigned char r, unsigned char g, unsigned char b)
{
    color_t alpha = 0xff;
    return alpha | r<<16 | g<<8 | b;
}

void gl_clear(color_t c)
{
    unsigned char *cptr = fb_get_draw_buffer();
    int nbytes = gl_get_width()*gl_get_height();
    memset(fb_get_draw_buffer, c nbytes); // fill entire framebuffer with pixels of color c
}

void gl_draw_pixel(int x, int y, color_t c)
{
    if (x >= gl_get_width() && y >= gl_get_height()) {
		return;
	}
    unsigned int (*im)[gl_get_width()] = fb_get_draw_buffer();
    im[y][x] = c; 
}

color_t gl_read_pixel(int x, int y)
{
    if (x >= gl_get_width() && y >= gl_get_height()) {
		return 0;
	}
    unsigned int (*im)[gl_get_width()] = fb_get_draw_buffer();
    return im[y][x];
}

void gl_draw_rect(int x, int y, int w, int h, color_t c)
{
    for( int i = 0; i < w; i ++ ) {
        for( int j = 0; j < h; j ++) {
            draw_pixel(i + x, j + y, c);
		}
    }
}

void gl_draw_char(int x, int y, char ch, color_t c)
{
    // TODO: implement this function
}

void gl_draw_string(int x, int y, const char* str, color_t c)
{
    // TODO: implement this function
}

unsigned int gl_get_char_height(void)
{
    // TODO: implement this function
    return 0;
}

unsigned int gl_get_char_width(void)
{
    // TODO: implement this function
    return 0;
}
