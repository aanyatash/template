#include "gpio.h"

struct gpio {
    unsigned int fsel[6];
    unsigned int reservedA;
    unsigned int set[2];
    unsigned int reservedB;
    unsigned int clr[2];
    unsigned int reservedC;
    volatile unsigned int lev[2];
};

static volatile struct gpio *gpio = (struct gpio *)0x20200000;

void gpio_init(void) {
    // no initialization required for this peripheral
}

void gpio_set_function(unsigned int pin, unsigned int function) {
   if (pin >= GPIO_PIN_FIRST && pin <= GPIO_PIN_LAST) {
  	 unsigned int fsel_register = pin / 10;
  	 unsigned int fsel_pin = (pin % 10)*3;
  	 unsigned int current = gpio->fsel[fsel_register];
  	 unsigned int reset = ~(0b111 << fsel_pin);
  	 unsigned int preserve = current & reset;
  	 gpio->fsel[fsel_register] = preserve | (function << fsel_pin);
   }
}

unsigned int gpio_get_function(unsigned int pin) {
    if (pin < GPIO_PIN_FIRST || pin > GPIO_PIN_LAST) {
    	return GPIO_INVALID_REQUEST;
    }
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
    if (pin < GPIO_PIN_FIRST || pin > GPIO_PIN_LAST || (value != 0 && value != 1)) {
	return;
    }
    unsigned int register_val = pin / 32;
    unsigned int bit_loc = pin;
    if (pin >= 32) {
   	 bit_loc = pin - 32;
    }
    if (value == 1) {
	gpio->set[register_val] = 1 << bit_loc;
    }
    else {
    	gpio->clr[register_val] = 1 << bit_loc;
    }
}

unsigned int gpio_read(unsigned int pin) {  
    if (pin < GPIO_PIN_FIRST || pin > GPIO_PIN_LAST) {
    	return GPIO_INVALID_REQUEST;
    }
    unsigned int register_val = pin / 32;
    unsigned int bit_loc = pin;
    if (pin >= 32) {
   	 bit_loc = pin - 32;
    }
    unsigned int binary = gpio->lev[register_val];
    unsigned int reset = 0b1 << bit_loc;
    unsigned int result = reset & binary;
    result = result >> bit_loc;
    return result;
}
