#include "assert.h"
#include "console.h"
#include "fb.h"
#include "gl.h"
#include "printf.h"
#include "strings.h"
#include "timer.h"
#include "uart.h"


void test_fb(void)
{
    const int SIZE = 500;
    const int DEPTH = 4;
    fb_init(SIZE, SIZE, DEPTH, FB_SINGLEBUFFER);

    assert(fb_get_width() == SIZE);
    assert(fb_get_height() == SIZE);
    assert(fb_get_depth() == DEPTH);
    assert(fb_get_pitch() >= fb_get_depth()*fb_get_width()); // consider why >= and not exact ==

    unsigned char *cptr = fb_get_draw_buffer();
    assert(cptr != NULL);
    int nbytes = fb_get_pitch()*fb_get_height();
    memset(cptr, 0x99, nbytes); // fill entire framebuffer with light gray pixels
    timer_delay(2);

    fb_init(600, 400, DEPTH, FB_DOUBLEBUFFER);
    cptr = fb_get_draw_buffer();  // virtual buffer
    nbytes = fb_get_pitch()*fb_get_height();
    memset(cptr, 0xff, nbytes); // fill hidden buffer with white pixels
    fb_swap_buffer(); // show white buffer
	timer_delay(1);

    cptr = fb_get_draw_buffer(); // virtual buffer
    memset(cptr, 0x33, nbytes); // fill other buffer with dark gray pixels
    fb_swap_buffer(); // show grey buffer
    timer_delay(1);

    for (int i = 0; i < 5; i++) {
        fb_swap_buffer(); // white, grey, virtual, normal, virtual, Should display dark grey three times
        timer_delay(1);
    }
}

void test_gl(void)
{
    const int WIDTH = 640;
    const int HEIGHT = 512;

    // Double buffer mode, make sure you test single buffer too!
    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);
    assert(gl_get_height() == HEIGHT);
    assert(gl_get_width() == WIDTH);

    // Background is purple
    gl_clear(gl_color(0x55, 0, 0x55));

    // Draw green pixel at an arbitrary spot
    gl_draw_pixel(WIDTH/3, HEIGHT/3, GL_GREEN);
    assert(gl_read_pixel(WIDTH/3, HEIGHT/3) == GL_GREEN);

    // Blue rectangle in center of screen
    gl_draw_rect(WIDTH/2 - 50, HEIGHT/2 - 50, 100, 100, GL_BLUE);

    // Single amber character
    gl_draw_char(60, 10, 'A', GL_AMBER);

    // Show buffer with drawn contents
    gl_swap_buffer();

	timer_delay(2);
    
	// Normal string drawing
    gl_clear(gl_color(0x55, 0, 0x55));
	gl_draw_string(60, 10, "Aanya Tashfeen", GL_AMBER);
	gl_swap_buffer(); // show
	timer_delay(3);

	// write past end
    gl_clear(gl_color(0x55, 0, 0x55));
	gl_draw_string(0, 0, "I think that this sentence should be way too long to print", GL_AMBER);
	gl_swap_buffer(); // show
	timer_delay(3);


	// write negative coordinates for string
    gl_clear(gl_color(0x55, 0, 0x55));
	gl_draw_string(-60, 10, "I think that this sentence should be way too long to print", GL_AMBER);

	// Negative coordinates for char
    gl_draw_char(-60, 10, 'S', GL_AMBER); // should show nothing
	gl_swap_buffer(); // show
	timer_delay(3);

	// write negative coordinates for pixel
    gl_clear(gl_color(0x55, 0, 0x55));
	gl_draw_pixel(60, -10, GL_AMBER);
	gl_swap_buffer(); // show
	timer_delay(3);

	// write negative for read
    color_t c = gl_read_pixel(-19, 2);	
	assert(c == 0);

	// write too big y for read
    c = gl_read_pixel(19, 2000);	
	assert(c == 0);

	// too big x for read
    c = gl_read_pixel(19000, 2);	
	assert(c == 0);

	// too big x and y for read
    c = gl_read_pixel(19000, 20000);	
	assert(c == 0);

    gl_clear(GL_INDIGO);
	// write too big y for pixel
	gl_draw_pixel(60, 100000, GL_AMBER);

	// too big x for pixel
	gl_draw_pixel(60000, 10, GL_AMBER);

	// too big x and y for pixel - edge case
	gl_draw_pixel(640, 512, GL_AMBER);

	gl_swap_buffer(); // show
	timer_delay(2);

    gl_clear(GL_INDIGO);
	// write too big y for string
	gl_draw_string(60, 100000, "Aanya Tashfeen", GL_AMBER);

	// too big x for string
	gl_draw_string(60000, 10, "Aanya Tashfeen", GL_AMBER);

	// too big x and y for string - edge case
	gl_draw_string(640, 512, "Aanya Tashfeen", GL_AMBER);

	gl_swap_buffer(); // show
	timer_delay(2);

	// intialize negative width or height
	// only spaces
    gl_draw_string(60, 10, "            .", GL_AMBER);
	gl_swap_buffer();
	timer_delay(2);

	// When font_get_glyph is false, so invalid char, nothing should happen
    gl_draw_char(60, 10, '\n', GL_AMBER);
	gl_swap_buffer();

	// clear after
	// test if pixel writes beyond, also rectangle, and char, string

    // negative colors should automatically be converted to signed with no
	// additonal processing needed
	gl_draw_char(60, 10, 'A', GL_AMBER);
	gl_clear(-5);
	gl_swap_buffer();
	timer_delay(1);

	gl_clear(GL_INDIGO);
	gl_swap_buffer();
	timer_delay(1);
	gl_clear(gl_color(-1, -2, -3));
	gl_swap_buffer();

	// test with single buffer too
    gl_init(WIDTH, HEIGHT, GL_SINGLEBUFFER);
	assert(gl_get_width() == WIDTH);
	assert(gl_get_height() == HEIGHT);
	gl_clear(GL_BLACK);
	gl_draw_pixel(60, 10, GL_GREEN);
	timer_delay(1);
	gl_draw_char(120, 20, 'A', GL_GREEN);
	timer_delay(1);
	gl_draw_string(240, 40, "HI", GL_GREEN);
	timer_delay(1);

	gl_clear(GL_SILVER);
	timer_delay(1);

	gl_draw_string(60, 10, "Aanya Tashfeen", GL_MAGENTA);
	timer_delay(1);

}

void test_basic_console()
{
    console_init(10, 30, GL_CYAN, GL_INDIGO);

    // Line 1: Hello, world!
    console_printf("Hello, world!\nHi\b");
    timer_delay(2);


    console_printf("Hello, world!\nHi\b");

}

void test_console(void)
{
    console_init(10, 30, GL_CYAN, GL_INDIGO);

    // Line 1: Hello, world!
    console_printf("Hello, world!\n");
    timer_delay(5);

    // Add line 2: Happiness == CODING
    console_printf("Happiness");
    console_printf(" == ");
    console_printf("CODING\n");
    timer_delay(2);

    // Add 2 blank lines and line 5: I am Pi, hear me roar!
    console_printf("\n\nI am Pi, hear me v\b \broar!\n"); // typo, backspace, correction
    timer_delay(2);

    // Clear all lines
    console_printf("\f");

    // Line 1: "Goodbye"
    console_printf("Goodbye!\n");
	timer_delay(3);

    // Horizontal wrapping
    console_printf("Hello, my name is Aanya Tashfeen. "); //Theoretically, this line should be long enough to wrap around.");
	timer_delay(3);

    // Scrolling
    console_printf("\n\n\n\n\n\n\nHello, my name is Aanya Tashfeen. "); //Theoretically, this line should be long enough to wrap around.");
	timer_delay(3);

// just entering

}

/* TODO: Add tests to test your graphics library and console.
   For the graphics library, test both single & double
   buffering and confirm all drawing is clipped to bounds
   of framebuffer
   For the console, make sure to test wrap of long lines and scrolling.
   Be sure to test each module separately as well as in combination
   with others.
*/

void main(void)
{
    uart_init();
    timer_init();
    printf("Executing main() in test_gl_console.c\n");

    //test_fb();
    test_gl();
	//test_basic_console();
    //test_console();

    printf("Completed main() in test_gl_console.c\n");
    uart_putchar(EOT);
}
