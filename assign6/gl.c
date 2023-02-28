#include "gl.h"
#include "fb.h"
#include "strings.h"
#include "font.h"
#include "printf.h"

static gl_mode_t swapped = 1;
static unsigned int full_width;


void gl_init(unsigned int width, unsigned int height, gl_mode_t mode)
{
    fb_init(width, height, 4, mode);    // use 32-bit depth always for graphics library
    full_width = fb_get_pitch()/fb_get_depth(); // this is the width including padding
}

void gl_swap_buffer(void)
{
    fb_swap_buffer();
}

unsigned int gl_get_width(void)
{
    return fb_get_width();
}

unsigned int gl_get_height(void)
{
    return fb_get_height();
}

color_t gl_color(unsigned char r, unsigned char g, unsigned char b)
{
    return 0xff000000 | r<<16 | g<<8 | b;
}

void gl_clear(color_t c)   // fix to make it pixel by pixel
{
    unsigned int (*im)[full_width] = fb_get_draw_buffer(); // gets virtual buffer
    for( int i = 0; i < gl_get_width(); i ++ ) {
        for( int j = 0; j < gl_get_height(); j ++) {
		    im[j][i] = c;
        }
    }

 // fill entire framebuffer with pixels of color c
}

void gl_draw_pixel(int x, int y, color_t c)
{
    if (x >= gl_get_width() && y >= gl_get_height()) {
		return;
	}
    unsigned int (*im)[full_width] = fb_get_draw_buffer();
    im[y][x] = c; 
}

color_t gl_read_pixel(int x, int y)
{
    if (x >= gl_get_width() && y >= gl_get_height()) {
		return 0;
	}
    unsigned int (*im)[full_width] = fb_get_draw_buffer();
    return im[y][x];
}

void gl_draw_rect(int x, int y, int w, int h, color_t c)
{
    for( int i = 0; i < w; i ++ ) {
        for( int j = 0; j < h; j ++) {
            gl_draw_pixel(i + x, j + y, c);
		}
    }
}

void gl_draw_char(int x, int y, char ch, color_t c)
{
    unsigned char buf[font_get_glyph_size()];
	memset(buf, '\0', sizeof(buf));
    font_get_glyph(ch, buf, sizeof(buf));


    // declare a variable `img` of proper type to point
    // to a two-dimensional array of glyph width/height
    // and initialize img to point to buf

    // after your addition, code below will print the glyph as
    // "ascii art" using # and space characters
	unsigned char (*img)[font_get_glyph_width()] = (unsigned char (*) [font_get_glyph_width()])buf;

    for (int i = 0; i < font_get_glyph_height(); i++) {
        for (int j = 0; j < font_get_glyph_width(); j++) {
			if (img[i][j] == 0xff) {
			    gl_draw_pixel(j + x, i + y, c);
			}
        }
    }

}

void gl_draw_string(int x, int y, const char* str, color_t c)
{
    int i = 0;
    while (str[i]) {
        gl_draw_char(x, y, str[i], c);
		x += font_get_glyph_width();
		i++;
	}
}

unsigned int gl_get_char_height(void)
{
    return font_get_glyph_height();
}

unsigned int gl_get_char_width(void)
{
    return font_get_glyph_width();
}
