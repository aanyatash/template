#include "console.h"
#include "gl.h"
#include "printf.h"
#include "strings.h"
#include "malloc.h"
#include "timer.h"

// Please use this amount of space between console rows
const static int LINE_SPACING = 5;
static int line_height;
static color_t background_color;
static color_t foreground_color;
static unsigned int cursor_x = 0;
static unsigned int cursor_y = 0;
static unsigned int** img;
static void scroll_clear(void);

void console_init(unsigned int nrows, unsigned int ncols, color_t foreground, color_t background)
{
    line_height = gl_get_char_height() + LINE_SPACING;
    gl_init(gl_get_char_width()*ncols, line_height*nrows, GL_DOUBLEBUFFER);
	background_color = background;
	foreground_color = foreground;
	img = (unsigned int**) malloc(gl_get_height()*4); // equivalent to height in pixels, 4 bytes per pixel
	for (int i = 0; i < gl_get_height(); i++) {
		img[i] = (unsigned int *) malloc(gl_get_width()*4); // equivalent to width
	}
	gl_clear(background);
	for (int i = 0; i < gl_get_width(); i ++) {
		for (int j = 0; j < gl_get_height(); j ++) {
		    img[j][i] = background_color;
		}
	}
	gl_swap_buffer();

}

void console_clear(void)
{
    gl_clear(background_color);
	cursor_x = 0;
	cursor_y = 0;
	for (int i = 0; i < gl_get_width(); i ++) {
		for (int j = 0; j < gl_get_height(); j ++) {
		    img[j][i] = background_color;
		}
	}
}

void scroll_clear(void)
{
    gl_clear(background_color);
}

static void process_char(char ch)
{
    // TODO: implement this helper function (recommended)
    // if ordinary char: insert ch into contents at current cursor position
    // and advance cursor
    // else if special char, do special handling
    if (ch == '\n') {
	    cursor_y += line_height;
		cursor_x = 0;
    }
    else if (ch == '\f') {
	    console_clear();
    }
    else if (ch == '\b') {
//	    if (cursor_x == 0) {
//		    cursor_y -= line_height;
//			cursor_x = gl_get_width() - 1;   // i think you may have to figure out how to turn all pixels to background color in rectangle glyph height/width
//			gl_draw_rect(cursor_x, cursor_y, gl_get_char_width(), gl_get_char_height(), background_color);
//		}
	    cursor_x -= gl_get_char_width();
		gl_draw_rect(cursor_x, cursor_y, gl_get_char_width(), gl_get_char_height(), background_color);
        for( int i = 0; i < gl_get_char_width(); i ++ ) {
            for( int j = 0; j < gl_get_char_height(); j ++) {
				img[j + cursor_y][i + cursor_x] = background_color;
		    }
        }
    }
    else {
	    if (cursor_x >= gl_get_width() - 1) {
		    cursor_y += line_height;
			cursor_x = 0;
		}
		if (cursor_y >= gl_get_height() - 1) {
		    scroll_clear();
		   	for (int i = 0; i < gl_get_width(); i ++) {
		        for (int j = line_height; j < gl_get_height(); j ++) {
				    gl_draw_pixel(i, j - line_height, img[j][i]);
				}
	        }
		   	for (int i = 0; i < gl_get_width(); i ++) {
		        for (int j = 0; j < gl_get_height(); j ++) {
				    img[j][i] = gl_read_pixel(i, j);
				}
	        }
			cursor_y -= line_height;
		}
	    gl_draw_char(cursor_x, cursor_y, ch, foreground_color);
        for( int i = 0; i < gl_get_char_width(); i ++ ) {
            for( int j = 0; j < gl_get_char_height(); j ++) {
			    if (gl_read_pixel(i + cursor_x, j + cursor_y) == foreground_color) {
				   img[j + cursor_y][i + cursor_x] = foreground_color;
				}
		    }
        }
	    cursor_x += gl_get_char_width();
		// wrap around
		// scrolling - memcpy into buf and then set i to 0 to write into new buf on virtual buf - only swapped once finished, track this!
    }

}

int console_printf(const char *format, ...)
{
    char buf[1024];
	size_t bufsize = sizeof(buf);
	memset(buf, '\0',  bufsize); // Initialize memory to nullptrs
    va_list args;
	va_start(args, format); // indicates that list of args starts after format param
	int total = vsnprintf(buf, bufsize, format, args);
	va_end(args);
    
	int i = 0;
	char ch;
	while (buf[i]) {
	    // iterate through img and draw everything and change cursors, then add the buffer stuff
		if (i == 0) {
		   	for (int i = 0; i < gl_get_width(); i ++) {
		        for (int j = 0; j < gl_get_height(); j ++) {
				    gl_draw_pixel(i, j, img[j][i]);
				}
	        }
			printf("x: %d\n", cursor_x);
			printf("y: %d\n", cursor_y);
		}
	    ch = buf[i];
		process_char(ch);
//		if (buf[i] != '\n') {
//		    gl_swap_buffer();
//		}
//		if (scroll == 1 && swap == 0) {
//		    i = 0;
//			swap = 1;
//		}
//		if (cur == i) {
//		    scroll = 0;
//		}
//		else {
//		    i++;
//		}
        i++;
	}
	gl_swap_buffer();
	printf("x after: %d\n", cursor_x);
    printf("y after: %d\n", cursor_y);
	return total;

}


