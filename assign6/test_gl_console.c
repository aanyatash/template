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
    timer_delay(3);

    fb_init(600, 400, DEPTH, FB_DOUBLEBUFFER);
    cptr = fb_get_draw_buffer();
    nbytes = fb_get_pitch()*fb_get_height();
    memset(cptr, 0xff, nbytes); // fill one buffer with white pixels
    fb_swap_buffer();

    cptr = fb_get_draw_buffer();
    memset(cptr, 0x33, nbytes); // fill other buffer with dark gray pixels
    fb_swap_buffer();
    timer_delay(1);

    for (int i = 0; i < 5; i++) {
        fb_swap_buffer();
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
    timer_delay(3);
}

void test_console(void)
{
    console_init(10, 30, GL_CYAN, GL_INDIGO);

    // Line 1: Hello, world!
    console_printf("Hello, world!\n");
    timer_delay(2);

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

    test_fb();
    //test_gl();
    //test_console();

    printf("Completed main() in test_gl_console.c\n");
    uart_putchar(EOT);
}
