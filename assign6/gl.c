/* Name: Aanya Tashfeen
 * Filename: gl.c
 * This file contains the code for initializing a graphics library with basic functionality
 * such as pixel, rectangle, character, and string drawing. This file interfaces with the
 * low-level framebuffer library and provides wrapper functions for some of the useful
 * framebuffer functions where global client access may be needed.
 */
#include "gl.h"
#include "fb.h"
#include "strings.h"
#include "font.h"
#include "printf.h"

static gl_mode_t swapped = 1;
static unsigned int full_width;

/* This function initializes the graphics library by initializing the framebuffer. It
 * takes 3 parameters:
 * 1) width is an unsigned integer describing the requested width in pixels of the framebuffer
 * 2) height is an unsigned integer describing the requested height in pixels of the framebuffer
 * 3) mode is a gl_mode_t enumerate that describes whether the framebuffer is in single buffered
 * (GL_SINGLEBUFFER) mode or double buffered (GL_DOUBLEBUFFER) mode and the function will
 * initialize the graphics library and framebuffer accordingly.
 */
void gl_init(unsigned int width, unsigned int height, gl_mode_t mode)
{
    fb_init(width, height, 4, mode);    // use 32-bit depth always for graphics library
    full_width = fb_get_pitch()/fb_get_depth(); // this is the width including padding
}

/* This function swaps buffers if in double buffering mode. All drawing will take place
 * on the off-screen buffer and the updated drawing is not displayed until this function
 * is called. If in single buffering mode, all drawing takes place on the the on-screen
 * buffer and this function has no effect.
 */
void gl_swap_buffer(void)
{
    fb_swap_buffer();
}

/* Returns the current width in pixels of the framebuffer as an unsigned int.
 */
unsigned int gl_get_width(void)
{
    return fb_get_width();
}

/* Returns the current height in pixels of the framebuffer as an unsigned int.
 */
unsigned int gl_get_height(void)
{
    return fb_get_height();
}

/* This returns a color composed of the specified red, green, and blue components and returns
 * the color as a single value of 32-bit type color_t.
 */
color_t gl_color(unsigned char r, unsigned char g, unsigned char b)
{
    // blue is the LSB
    return 0xff000000 | r<<16 | g<<8 | b;
}

/* This function clears all the pixels in the framebuffer to the given color.
 */
void gl_clear(color_t c)
{
    unsigned int (*im)[full_width] = fb_get_draw_buffer(); // gets virtual buffer
	// fill entire frame buffer with pixels of color c
	// avoids writing into the padding of the framebuffer
    for( int i = 0; i < gl_get_width(); i ++ ) {
        for( int j = 0; j < gl_get_height(); j ++) {
		    im[j][i] = c;
        }
    }

}

/* This function draws one pixel at coordinates x, y in the color c. It takes
 * 3 parameters:
 * 1) x is an integer coordinate on the framebuffer
 * 2) y is an integer coordinate on the framebuffer
 * 3) c is a color_t that described the desired color of the pixel
 * This function will do nothing if the x and y coordinates are outside of the
 * bounds of the framebuffer.
 */
void gl_draw_pixel(int x, int y, color_t c)
{
    // Does nothing if coords outside bounds of framebuffer
    if (x >= gl_get_width() && y >= gl_get_height() && x < 0 && y < 0) {
		return;
	}
	// Gets pointer to framebuffer and makes a 2d array
    unsigned int (*im)[full_width] = fb_get_draw_buffer();
    im[y][x] = c; 
}

/* This function returns the color of the [ixel at the coordinates x, y. It takes
 * 2 parameters:
 * 1) x is an integer coordinate on the framebuffer
 * 2) y is an integer coordinate on the framebuffer
 * The function returns the color of the pixel at that coordinate point as a color_t. It
 * returns 0 if the coordinates are outside the bounds of the framebuffer.
 */
color_t gl_read_pixel(int x, int y)
{
    // Returns 0 if outside bounds of framebuffer
    if (x >= gl_get_width() && y >= gl_get_height() && x < 0 && y < 0) {
		return 0;
	}
	// Pointer to 2d framebuffer array
    unsigned int (*im)[full_width] = fb_get_draw_buffer();
    return im[y][x];
}

/* This function draws a filled rectangle at the coordinate x, y with a width
 * 'w' and a height 'h'.
 */
void gl_draw_rect(int x, int y, int w, int h, color_t c)
{
    for( int i = 0; i < w; i ++ ) {
        for( int j = 0; j < h; j ++) {
            gl_draw_pixel(i + x, j + y, c);
		}
    }
}

/* This function draws a single character at coordinates x, y in color c.
 * Only pixels in the bounds of framebuffer are drawn. The function takes
 * 4 parameters:
 * 1) x is an integer coordinate on the framebuffer
 * 2) y is an integer coordinate on the framebuffer
 * 3) ch is the char to be drawn
 * 4) c is a color_t that described the desired color of the pixel
 */
void gl_draw_char(int x, int y, char ch, color_t c)
{
    unsigned char buf[font_get_glyph_size()];
	memset(buf, '\0', sizeof(buf));
    font_get_glyph(ch, buf, sizeof(buf)); // add char info to buf

    // declare a variable `img` of proper type to point
    // to a two-dimensional array of glyph width/height
    // and initialize img to point to buf
	unsigned char (*img)[font_get_glyph_width()] = (unsigned char (*) [font_get_glyph_width()])buf;

    // Draws 'on' pixels into img
	// Iterates through pixel by pixel
    for (int i = 0; i < font_get_glyph_height(); i++) {
        for (int j = 0; j < font_get_glyph_width(); j++) {
			if (img[i][j] == 0xff) {
			    gl_draw_pixel(j + x, i + y, c);
			}
        }
    }

}

/* This function draws a string at the coordinates x, y in the color c. The characters
 * are drawn left to write in a single line. Only pixels within framebuffer bounds are drawn.
 * The function takes 4 parameters:
 * 1) x is an integer coordinate on the framebuffer
 * 2) y is an integer coordinate on the framebuffer
 * 3) str is the string of chars to be drawn
 * 4) c is a color_t that described the desired color of the pixel
 */
void gl_draw_string(int x, int y, const char* str, color_t c)
{
    int i = 0;
	// Draw chars until end of string reached
    while (str[i]) {
        gl_draw_char(x, y, str[i], c);
		x += font_get_glyph_width(); // calculates width to draw chars spaced apart
		i++;
	}
}

/* Returns the height in pixels of a character glyph as an unsigned int
 */
unsigned int gl_get_char_height(void)
{
    return font_get_glyph_height();
}

/* Returns the width in pixels of a character glyph as an unsigned int
 */
unsigned int gl_get_char_width(void)
{
    return font_get_glyph_width();
}
