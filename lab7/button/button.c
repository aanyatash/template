#include "gl.h"
#include "gpio.h"
#include "gpio_extra.h"
#include "gpio_interrupts.h"
#include "interrupts.h"
#include "printf.h"
#include "ringbuffer.h"
#include "uart.h"

static const int BUTTON = GPIO_PIN20;
static int gCount = 0;

void wait_for_click(void) {
    // TODO: implement this function
}

void redraw(int nclicks) {
    static int nredraw = -1;
    // count number of redraws, alternate bg color on each redraw
    color_t bg = nredraw++ % 2 ? GL_AMBER : GL_BLUE;

    gl_clear(GL_BLACK);
    char buf[100];
    snprintf(buf, sizeof(buf), "Click count = %d (redraw #%d)", nclicks, nredraw);
    gl_draw_string(0, 0, buf, GL_WHITE);
    for (int y = gl_get_char_height(); y < gl_get_height(); y++) {
        for (int x = 0; x < gl_get_width(); x++) {
            gl_draw_pixel(x, y, bg);
        }
    }
}

void main(void) {
    gpio_init();
    uart_init();
    gl_init(600, 400, GL_SINGLEBUFFER);

    gpio_set_input(BUTTON); // configure button
    gpio_set_pullup(BUTTON);

    int drawn = -1;
    while (true) {
        drawn = gCount;
        redraw(drawn);
        wait_for_click();
    }
}
