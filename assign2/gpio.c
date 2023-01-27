#include "gpio.h"

struct gpio {
    unsigned int fsel[6];
    unsigned int reservedA;
    unsigned int set[6];
    unsigned int reservedB;
    unsigned int clr[2];
    unsigned int reservedC;
    unsigned int lev[2];
};


//static unsigned int *FSEL =  (unsigned int *)0x20200000;
//static unsigned int *SET0  =  (unsigned int *)0x2020001c;
//static unsigned int *CLR0  =  (unsigned int *)0x20200028;
//static unsigned int *LEV0  =  (unsigned int *)0x20200034;

static volatile struct gpio *gpio = (struct gpio *)0x20200000;

void gpio_init(void) {
    // no initialization required for this peripheral
}

void gpio_set_function(unsigned int pin, unsigned int function) {
   unsigned int fsel_register = pin / 10;
   unsigned int fsel_pin = (pin % 10)*3;
   unsigned int current = gpio->fsel[fsel_register];
   unsigned int reset = ~(0b111 << fsel_pin);
   unsigned int preserve = current & reset;
   gpio->fsel[fsel_register] = preserve | (function << fsel_pin);
}

unsigned int gpio_get_function(unsigned int pin) {
    unsigned int fsel_register = pin / 10;
    unsigned int fsel_pin = (pin % 10)*3;
    unsigned int binary =  gpio->fsel[fsel_register];
    unsigned int reset = (0b111 << fsel_pin);
    unsigned int result = reset & binary;
    result = result >> fsel_pin;
    return result;

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
