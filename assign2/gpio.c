#include "gpio.h"

void gpio_init(void) {
    // no initialization required for this peripheral
}

void gpio_set_function(unsigned int pin, unsigned int function) {
   // TODO: Your code goes here.
}

unsigned int gpio_get_function(unsigned int pin) {
    return 0;  // TODO: Your code goes here.
}

void gpio_set_input(unsigned int pin) {
    gpio_set_function(pin, GPIO_FUNC_INPUT);
}

void gpio_set_output(unsigned int pin) {
    gpio_set_function(pin, GPIO_FUNC_OUTPUT);
}

void gpio_write(unsigned int pin, unsigned int value) {
    // TODO: Your code goes here.
}

unsigned int gpio_read(unsigned int pin) {
    return 0;  // TODO: Your code goes here.
}
