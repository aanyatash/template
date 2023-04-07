/* Name: Aanya Tashfeen
 * Filename: "ps2.c"
 * This file contains code that initializes the keyboard and then reads the data and clock
 * to identify the key presses. The file also uses the parity, stop, and start bit obtained 
 * from data as an error-correcting code.
 */

#include "gpio.h"
#include "gpio_extra.h"
#include "malloc.h"
#include "ps2.h"
#include "uart.h"
#include "printf.h"

// A ps2_device is a structure that stores all of the state and information
// needed for a PS2 device. The clock field stores the pin number for the
// clock GPIO pin, and the data field stores the pin number for the data
// GPIO pin. ps2_new shows examples of how these fields are used.
//
// You may extend this structure with additional fields. As
// it is passed into all ps2_ calls, storing state in this structure is
// preferable to using global variables: it allows your driver to
// communicate with more than one concurrent PS2 device (e.g., a keyboard
// and a mouse).
//
// This definition fills out the structure declared in ps2.h.
struct ps2_device {
    unsigned int clock;
    unsigned int data;
};

// Creates a new PS2 device with a particular clock and data pin,
// initializing pins to be pull-up inputs.
ps2_device_t *ps2_new(unsigned int clock_gpio, unsigned int data_gpio)
{
    // consider why must malloc be used to allocate device
    ps2_device_t *dev = malloc(sizeof(*dev));

    dev->clock = clock_gpio;
    gpio_set_input(dev->clock);
    gpio_set_pullup(dev->clock);

    dev->data = data_gpio;
    gpio_set_input(dev->data);
    gpio_set_pullup(dev->data);
    return dev;
}

/* This function takes in the clock as an unsigned integer argument and
 * waits for the clock edge to fall as this indicates that data is being sent.
 */
void wait_for_falling_clock_edge(unsigned int clk)
{
    while(gpio_read(clk) == 0) {}
    while(gpio_read(clk) == 1) {}
}

// This function reads a single ps2 scan code. It always returns a correctly received scan code:
// if an error occurs (e.g., start bit not detected, parity is wrong), the
// function should read another scan code.
unsigned char ps2_read(ps2_device_t *dev)
{
    while (1) {
	    unsigned int number = 0;
		unsigned int parity = 0;
		unsigned int bit = 0;
		while (1) {
		    wait_for_falling_clock_edge(dev->clock);
			// This ensures that the start bit is correctly
			// a 0.
			if (gpio_read(dev->data) == 0) {
				break;
			}
		}
		// Reads the next 10 bits after start bit
		for (int i = 0; i < 10; i++) {
		    // Only reads data lines once the clock edge falls
		    wait_for_falling_clock_edge(dev->clock);
			// Next 8 bits received are the bits for data we want to use
			if (i < 8) {
			    bit = (gpio_read(dev->data) ? 1 : 0);
				parity += bit;
				number += bit << i; // LSB first received
			}
			// The 10th bit received is the parity bit
			if (i == 8) {
			    parity += (gpio_read(dev->data) ? 1 : 0);
				// Total number of ones must be odd, if it isn't
				// discrad the bits and start reading again
				if (parity % 2 == 0) {
				    break;
				}
			}
			// The 11th bit recieved must be the stop bit and must be 1
			// If it is not, loop starts over and data should be read again
			if (i == 9) {
				if (gpio_read(dev->data) == 1) {
				    return number;
				}
			}
		}
    }
    return 0xFF;
}


