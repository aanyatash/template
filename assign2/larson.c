/* File: larson.c
 * --------------
 * Sample program that demonstrates use of gpio and timer library modules.
 * (Larson scanner rewritten in C)
 */

#include "gpio.h"
#include "timer.h"

static const int NUM_LEDS = 8;
static const int START_GPIO = 20;
static const int DELAY_MS = 1000/NUM_LEDS;  // frequency 1 sweep per sec

void blink(int pin) {
    gpio_write(pin, 1);
    timer_delay_ms(DELAY_MS);
    gpio_write(pin, 0);
    timer_delay_ms(DELAY_MS);
}

void main(void) {
    gpio_init();
    for (int i = 0; i < NUM_LEDS; i++) {
        gpio_set_output(START_GPIO + i);
    }

    unsigned int cur = START_GPIO;
    while (1) {
        for (int i = 0; i < NUM_LEDS-1; i++) {
            blink(cur++); // move up
        }
        for (int i = 0; i < NUM_LEDS-1; i++) {
            blink(cur--); // move down
        }
    }
}
