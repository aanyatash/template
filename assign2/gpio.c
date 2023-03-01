/* Name: Aanya Tashfeen
 * Filename: gpio.c
 * This file contains functions that can set GPIO pins to a certain function
 * using FSEL registers, return what function a certain GPIO pin is set to,
 * and also write to the SET and CLR registers for a specified pin, and read
 * the LEV register for a specified pin. The functions contain error handling
 * for pins that are out of range (less than 0 or greater than 54).
 */
#include "gpio.h"

// Initializes struct for gpio pins by specifying the number of registers for each
// type of register in an array.
struct gpio {
    unsigned int fsel[6];
    unsigned int reservedA;
    unsigned int set[2];
    unsigned int reservedB;
    unsigned int clr[2];
    unsigned int reservedC;
    volatile unsigned int lev[2];
};

// Specifies start memory address for gpio registers
static volatile struct gpio *gpio = (struct gpio *)0x20200000;

void gpio_init(void) {
    // no initialization required for this peripheral
}


/* This function sets a GPIO 'function' for a GPIO 'pin' without
 * changing the other pins functions. It takes the pin and function
 * as unsigned int arguments. It does nothing if the pin value is invalid.
 */
void gpio_set_function(unsigned int pin, unsigned int function) {
   if (pin >= GPIO_PIN_FIRST && pin <= GPIO_PIN_LAST && function >= 0 && function <= 7) {
  	 unsigned int fsel_register = pin / 10;  // Finds which FSEL register to look at
  	 unsigned int fsel_pin = (pin % 10)*3;  // Identifies the location of the bits to look at
  	 unsigned int current = gpio->fsel[fsel_register]; // Stores the current FSEL register bit value in current
         // Puts 000 in the location of the bit values of the FSEL register we want to change and 1's in the bits
         // of the bit values of the other pins we want to preserve
  	 unsigned int reset = ~(0b111 << fsel_pin); 
  	 unsigned int preserve = current & reset; // 000 in bit value location of FSEL register we want to change, eveything else is preserved
  	 gpio->fsel[fsel_register] = preserve | (function << fsel_pin); // Changes function bit value of FSEL register we want to change
   }
}

/* This function returns the GPIO function of a specified 'pin' and 
 * returns it as an unsigned integer. Returns GPIO_INVALID_REQUEST if
 * pin number is invalid.
 */
unsigned int gpio_get_function(unsigned int pin) {
    if (pin < GPIO_PIN_FIRST || pin > GPIO_PIN_LAST) {
    	return GPIO_INVALID_REQUEST;
    }
    unsigned int fsel_register = pin / 10; // Finds which FSEL register to look at
    unsigned int fsel_pin = (pin % 10)*3; // Identifies location of bits to look at
    unsigned int binary =  gpio->fsel[fsel_register]; // Finds binary value stored in FSEL register
    unsigned int reset = (0b111 << fsel_pin); // reset is 111 at bit location of pin
    unsigned int result = reset & binary; // Preserves bit value at bit location of pin
    result = result >> fsel_pin; // Shifts 3-bit value to LSB so it can be returned
    return result;
}

/* Convenience function to set pin to input. Takes pin number as argument.
 */
void gpio_set_input(unsigned int pin) {
    gpio_set_function(pin, GPIO_FUNC_INPUT);
}

/* Convenience function to set pin to output. Takes pin number as argument.
 */
void gpio_set_output(unsigned int pin) {
    gpio_set_function(pin, GPIO_FUNC_OUTPUT);
}

/* This function writes to set registers is input 'value' is 1 and
 * to clr registers if input 'value' is 0. It writes to these registers
 * to set a specified 'pin' to high or low. It doesn't do anything if
 * an invalid pin or value (not 0 or 1) is given.
 */
void gpio_write(unsigned int pin, unsigned int value) {
    if (pin < GPIO_PIN_FIRST || pin > GPIO_PIN_LAST || (value != 0 && value != 1)) {
	return;
    }
    unsigned int register_val = pin / 32; // 32 pins per register for SET and CLR
    unsigned int bit_loc = pin; // bit location of pin number
    // Finds bit location of specified pin if pin is in second register i.e. pin >= 32
     if (pin >= 32) {
   	 bit_loc = pin - 32;
    }
    if (value == 1) {
	gpio->set[register_val] = 1 << bit_loc; // Sets pin to high with SET registers
    }
    else {
    	gpio->clr[register_val] = 1 << bit_loc; // Sets pin to low with CLR registers
    }
}

/* This function reads the lev register for a specified 'pin'. It returns
 * 1 or 0 depending on if the reading is high or low. the function returns
 * GPIO_INVALID_REQUEST is an invalid GPIO pin is specified.
 */
unsigned int gpio_read(unsigned int pin) {  
    if (pin < GPIO_PIN_FIRST || pin > GPIO_PIN_LAST) {
    	return GPIO_INVALID_REQUEST;
    }
    unsigned int register_val = pin / 32; // 32 pins per register for LEV registers
    unsigned int bit_loc = pin;
    if (pin >= 32) {
   	 bit_loc = pin - 32;
    }
    unsigned int binary = gpio->lev[register_val]; // Finds binary value stored in LEV register
    unsigned int reset = 1 << bit_loc;
    unsigned int result = reset & binary; // Extracts single bit value at desired location for pin
    result = result >> bit_loc; // Shifts resulting bit value to LSB so it can be returned
    return result;
}
