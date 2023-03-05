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
#include "gpio_interrupts.h"
#include "ringbuffer.h"

static void handle_ps2(unsigned int pc, void *dev);

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
	unsigned int calls;
	unsigned int number;
	unsigned int parity;
	unsigned int bit;
	rb_t *rb;
};

// Creates a new PS2 device with a particular clock and data pin,
// initializing pins to be pull-up inputs.
ps2_device_t *ps2_new(unsigned int clock_gpio, unsigned int data_gpio)
{
    // consider why must malloc be used to allocate device
    ps2_device_t *dev = malloc(sizeof(*dev));
	gpio_interrupts_init();

    dev->clock = clock_gpio;
	dev->rb = rb_new();
	dev->calls = -2;
	dev->number = 0;
	dev->parity = 0;
	dev->bit = 0;
    gpio_set_input(dev->clock);
    gpio_set_pullup(dev->clock);

    dev->data = data_gpio;
    gpio_set_input(dev->data);
    gpio_set_pullup(dev->data);

	gpio_enable_event_detection(dev->clock, GPIO_DETECT_FALLING_EDGE);
    gpio_interrupts_register_handler(dev->clock, handle_ps2, dev);
    gpio_interrupts_enable();

    return dev;
}

/* This function takes in the clock as an unsigned integer argument and
 * waits for the clock edge to fall as this indicates that data is being sent.
 */
//void wait_for_falling_clock_edge(unsigned int clk)
//{
//    while(gpio_read(clk) == 0) {}
//    while(gpio_read(clk) == 1) {}
//}

static void handle_ps2(unsigned int pc, void *dev){
    // check for clock falling edge
    if (gpio_check_and_clear_event(((ps2_device_t *) dev)->clock)) {
		((ps2_device_t *) dev)->calls += 1;
    }
	// if no falling edge, read again
	// do we have to reset?
	else {
		return;
	}
    if (((ps2_device_t *)dev)->calls == -1) {
		// This ensures that the start bit is correctly
    	// a 0.
		if (gpio_read(((ps2_device_t *) dev)->data) != 0) {
		     ((ps2_device_t *) dev)->calls = -2;
		}
    }
    // Reads the next 10 bits after start bit
	// dev->calls should be 0 and above here
    else {
		// Next 8 bits received are the bits for data we want to use
		if (((ps2_device_t *) dev)->calls < 8) {
		    ((ps2_device_t *) dev)->bit = (gpio_read(((ps2_device_t *) dev)->data) ? 1 : 0);
		    ((ps2_device_t *) dev)->parity += ((ps2_device_t *) dev)->bit;
			// LSB first received
		    ((ps2_device_t *) dev)->number += ((ps2_device_t *) dev)->bit << ((ps2_device_t *) dev)->calls; 
		}
		// The 10th bit received is the parity bit
		else if (((ps2_device_t *) dev)->calls == 8) {
		    ((ps2_device_t *) dev)->parity += (gpio_read(((ps2_device_t *) dev)->data) ? 1 : 0);
		    // Total number of ones must be odd, if it isn't
		    // discard the bits and start reading again
		    if (((ps2_device_t *) dev)->parity % 2 == 0) {
				((ps2_device_t *) dev)->calls = -2;
				((ps2_device_t *) dev)->number = 0;
				((ps2_device_t *) dev)->parity = 0;
				((ps2_device_t *) dev)->bit = 0;
		    }
		}
		// The 11th bit recieved must be the stop bit and must be 1
		// If it is not, loop starts over and data should be read again
		else if (((ps2_device_t *) dev)->calls == 9) {
		    if (gpio_read(((ps2_device_t *) dev)->data) == 1) {
				rb_enqueue(((ps2_device_t *) dev)->rb, ((ps2_device_t *) dev)->number);
		    }
 		    ((ps2_device_t *) dev)->calls = -2;
		    ((ps2_device_t *) dev)->number = 0;
		    ((ps2_device_t *) dev)->parity = 0;
		    ((ps2_device_t *) dev)->bit = 0;
		}
    }

//    if (gpio_check_and_clear_event(((ps2_device_t *) dev)->clock)) {
//		((ps2_device_t *) dev)->calls += 1;
//    }
}
// This function reads a single ps2 scan code. It always returns a correctly received scan code:
// if an error occurs (e.g., start bit not detected, parity is wrong), the
// function should read another scan code.
unsigned char ps2_read(ps2_device_t *dev)
{
    while (rb_empty(dev->rb)) {/* spin */ }
	int* temp = malloc(4);
	rb_dequeue(dev->rb, temp);
	free(temp);
    return *temp;
}

