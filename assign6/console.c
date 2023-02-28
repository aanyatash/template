/* Name: Aanya Tashfeen
 * Filename: console.c
 * This file contains the code to intialize a console and for a console printf function.
 * Console printf contains the functionality for horizontal wrapping and vertical scrolling.
 * The vertical scrolling in this file is not very optimized and so it takes about ~3 seconds to scroll.
 */
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
static int total = 0;

/* This function initializes the console. The console is intially empty and is cleared to the desired
 * background color. The cursor begins in the home position. This function takes 4 parameters:
 * 1) nrows is an unsigned int that describes the requested number of rows in chars
 * 2) ncols is an unsigned int that describes the requested number of cols in chars
 * 3) foreground is a color_t that describes the color for text.
 * 4) background is a color_t that describes the color of the background.
 */
void console_init(unsigned int nrows, unsigned int ncols, color_t foreground, color_t background)
{
    line_height = gl_get_char_height() + LINE_SPACING;
	// Initializes frambuffer based on requested no of rows and cols
    gl_init(gl_get_char_width()*ncols, line_height*nrows, GL_DOUBLEBUFFER);
	// Uses global static variables to keep track of colors requested
	background_color = background;
	foreground_color = foreground;

	// Allocates a 2D array to keep track of what's on the screen when console is initialized
	img = (unsigned int**) malloc(gl_get_height()*4); // equivalent to height in pixels, 4 bytes per pixel
	for (int i = 0; i < gl_get_height(); i++) {
		img[i] = (unsigned int *) malloc(gl_get_width()*4); // equivalent to width
	}

	// Clears console to requested background color
	gl_clear(background);

    // Stores this request for background color in the 2D array, img
	for (int i = 0; i < gl_get_width(); i ++) {
		for (int j = 0; j < gl_get_height(); j ++) {
		    img[j][i] = background_color;
		}
	}
	// Displays the background color
	gl_swap_buffer();
}

/* Clear all console test and move cursor to home position.
 */
void console_clear(void)
{
    gl_clear(background_color);
	cursor_x = 0;
	cursor_y = 0;
	// Store the changed console in 2D array, img
	for (int i = 0; i < gl_get_width(); i ++) {
		for (int j = 0; j < gl_get_height(); j ++) {
		    img[j][i] = background_color;
		}
	}
}

/* Clear all text on screen before rewriting to display a scroll effect. The 
 * rewriting is done in the process_ch function.
 */
void scroll_clear(void)
{
    gl_clear(background_color);
}

/* This helper function processes characters. If ordinary char, insert ch into contents at current cursor position
 * and advance cursor. Else if it's a special char, do special handling. This function also handles horizontal 
 * wrapping and vertical scrolling if text needs to be written beyond the edge of the console display. This 
 * function takes one parameter - the char 'ch' that needs to be processed.
 */
static void process_char(char ch)
{
    // Special char: '\n'
    if (ch == '\n') {
	    cursor_y += line_height;
		cursor_x = 0;
		// For vertical scrolling, when end of console is reached
		if (cursor_y >= gl_get_height() - 1) {
		    scroll_clear(); // remove all text
		    // screen is now blank, but we must redraw everything
			// we must also store new screen contents in our array
		   	for (int i = 0; i < gl_get_width(); i ++) {
		        for (int j = line_height; j < gl_get_height(); j ++) {
				    gl_draw_pixel(i, j - line_height, img[j][i]); // draw correctly scrolled contents into buffer
				    img[j - line_height][i] = gl_read_pixel(i, j - line_height); // store contents in img
				}
	        }
			// store last line's contents in 2D array, img (empty line)
		    for (int i = 0; i < gl_get_width(); i ++) {
			    for (int j = gl_get_height() - line_height; j < gl_get_height(); j ++) {
				    img[j][i] = gl_read_pixel(i, j);
			    }
		    }
			cursor_y -= line_height; // cursor_y should be moved upwards now that we've scrolled
		}
    }
	// Special char: '\f' - clear screen and reset cursor
    else if (ch == '\f') {
	    console_clear();
    }
	// Special char: '\b' - delete character and move cursor back
    else if (ch == '\b') {
	    // Move cursor back by one character
	    cursor_x -= gl_get_char_width();
		// Draw rectangle over character to delete
		gl_draw_rect(cursor_x, cursor_y, gl_get_char_width(), gl_get_char_height(), background_color);
		// Store the deletion in 2D array, img
        for( int i = 0; i < gl_get_char_width(); i ++ ) {
            for( int j = 0; j < gl_get_char_height(); j ++) {
				img[j + cursor_y][i + cursor_x] = background_color;
		    }
        }
    }
    else {
	    // For horizontal wrap when end of line is reached
	    if (cursor_x >= gl_get_width() - 1) {
		    cursor_y += line_height;
			cursor_x = 0;
		}
		// For vertical scrolling, when end of console is reached
		if (cursor_y >= gl_get_height() - 1) {
		    scroll_clear(); // remove all text
		    // screen is now blank, but we must redraw everything
			// we must also store new screen contents in our array
		   	for (int i = 0; i < gl_get_width(); i ++) {
		        for (int j = line_height; j < gl_get_height(); j ++) {
				    gl_draw_pixel(i, j - line_height, img[j][i]); // draw correctly scrolled contents into buffer
				    img[j - line_height][i] = gl_read_pixel(i, j - line_height); // store contents in img
				}
	        }
			// store last line's contents in 2D array, img (empty line)
		    for (int i = 0; i < gl_get_width(); i ++) {
			    for (int j = gl_get_height() - line_height; j < gl_get_height(); j ++) {
				    img[j][i] = gl_read_pixel(i, j);
			    }
		    }
			cursor_y -= line_height; // cursor_y should be moved upwards now that we've scrolled
		}
		// Draw char into framebuffer
	    gl_draw_char(cursor_x, cursor_y, ch, foreground_color);
		// Store this change in our 2D array, img
        for( int i = 0; i < gl_get_char_width(); i ++ ) {
            for( int j = 0; j < gl_get_char_height(); j ++) {
			    if (gl_read_pixel(i + cursor_x, j + cursor_y) == foreground_color) {
				   img[j + cursor_y][i + cursor_x] = foreground_color;
				}
		    }
        }
		// Move cursor to end of drawn char
	    cursor_x += gl_get_char_width();
    }

}

/* This function takes the same arguments as 'printf', so the format string and other parameters to be
 * inserted into the string. The function processes the string character by character and then displays
 * the entire string on the console. The function returns the total number of chars written to the console, or
 * intending to be written to the console.
 */
int console_printf(const char *format, ...)
{
    char buf[1024];
	size_t bufsize = sizeof(buf);
	memset(buf, '\0',  bufsize); // Initialize memory to nullptrs
    va_list args;
	va_start(args, format); // indicates that list of args starts after format param
	total += vsnprintf(buf, bufsize, format, args);
	va_end(args);
    
	int i = 0;
	char ch;
	// Process chars until the end of buf
	while (buf[i]) {
	    // if we're trying to print a new line, we must ensure that the console stays updated
		// read our storage 2D array, img, and draw each pixel onto screen
		if (i == 0) {
		   	for (int i = 0; i < gl_get_width(); i ++) {
		        for (int j = 0; j < gl_get_height(); j ++) {
				    gl_draw_pixel(i, j, img[j][i]);
				}
	        }
		}
	    ch = buf[i];
		process_char(ch);
        i++;
	}
	gl_swap_buffer(); // display processed string
	return total;
}


