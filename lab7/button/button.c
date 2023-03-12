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
static void interrupts_register_handler(unsigned int pin, handler_fn_t fn, void *aux_data); 


// This array stores the handlers for gpio interrupts, one per pin.
// If no handler is registered for a given pin, fn at that index is NULL
// global array in bss, values zero'ed at program start
static struct {
    handler_fn_t fn;
    void *aux_data;
} handlers[GPIO_PIN_LAST + 1];

void interrupts_register_handler(unsigned int pin, handler_fn_t fn, void *aux_data) {
    //assert(gpio_interrupts_initialized);
    //assert(pin <= GPIO_PIN_LAST);
    handlers[pin].fn = fn;
    handlers[pin].aux_data = aux_data;
	console_printf("%s", (char*) aux_data);
}

void wait_for_click(void) {
    while (gpio_read(GPIO_PIN20) == 1) 	{/* spin */}
	gCount += 1;
	printf("Click!");
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

    gpio_enable_event_detection(GPIO_PIN20, wait_for_click);
    interrupts_init();

    gpio_interrupts_init();
    interrupts_register_handler(GPIO_PIN20, wait_for_click, "aanya");
    gpio_interrupts_enable();

    interrupts_global_enable();

    int drawn = -1;
    while (true) {
        drawn = gCount;
        redraw(drawn);
        wait_for_click();
    }
}
