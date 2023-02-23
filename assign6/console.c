#include "console.h"
#include "gl.h"

// Please use this amount of space between console rows
const static int LINE_SPACING = 5;
static int line_height;

void console_init(unsigned int nrows, unsigned int ncols, color_t foreground, color_t background)
{
    line_height = gl_get_char_height() + LINE_SPACING;
    // TODO: implement this function
}

void console_clear(void)
{
    // TODO: implement this function
}

int console_printf(const char *format, ...)
{
    // TODO: implement this function, be sure to use your vsnprintf!
	return 0;
}

static void process_char(char ch)
{
    // TODO: implement this helper function (recommended)
    // if ordinary char: insert ch into contents at current cursor position
    // and advance cursor
    // else if special char, do special handling
}
