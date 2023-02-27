#include "console.h"
#include "gl.h"
#include "printf.h"

// Please use this amount of space between console rows
const static int LINE_SPACING = 5;
static int line_height;
static color_t background_color;
static color_t foreground_color;
static unsigned int cursor_x = 0;
static unsigned int cursor_y = 0;

void console_init(unsigned int nrows, unsigned int ncols, color_t foreground, color_t background)
{
    line_height = gl_get_char_height() + LINE_SPACING;
    gl_init(nrows, ncols, GL_DOUBLEBUFFER);
	gl_clear(background);
	background_color = background;
	foreground_color = foreground;
}

void console_clear(void)
{
    gl_clear(background_color);
	cursor_x = 0;
	curosr_y = 0;
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
    if (ch == '\f') {
	    console_clear();
    }
    if (ch == '\b') {
	    if (cursor_x == 0) {
		    cursor_y -= line_height;
			cursor_x = gl_get_width() - 1;
		}
	    cursor_x -= gl_get_char_width();
    }
    else {
	    gl_draw_ch(cursor_x, cursor_y, ch, c);
	    cursor_x += gl_get_char_width();
		// wrap around
    }
}

int console_printf(const char *format, ...)
{
    char buf[1024];
	size_t bufsize = sizeof(buf);
	memset(buf, '\0',  bufsize); // Initialize memory to nullptrs
    va_list args;
	va_start(args, iformat); // indicates that list of args starts after format param
	int total = vsnprintf(buf, bufsize, format, args);
	va_end(args);
    
	int i = 0;
	char ch;
	while (buf[i]) {
	    ch = buf[i];
		process_char(ch);
		i++;
	}

}


