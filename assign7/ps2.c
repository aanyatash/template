/* Name: Aanya Tashfeen
 * Filename: "ps2.c"
 * This file contains code that initializes the keyboard and then reads the data and clock
 * to identify the key presses. The file also uses the parity, stop, and start bit obtained 
 * from data as an error-correcting code. This code uses interrupts to read from the clock line.
 */

#include "gpio.h"
#include "gpio_extra.h"
#include "malloc.h"
#include "ps2.h"
#include "uart.h"
#include "printf.h"
#include "gpio_interrupts.h"
#include "ringbuffer.h"

static void handle_ps2(unsigned int pc, void *aux_data);

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
	rb_t *rb; // stores scancodes
};

// Creates a new PS2 device with a particular clock and data pin,
// initializing pins to be pull-up inputs.
ps2_device_t *ps2_new(unsigned int clock_gpio, unsigned int data_gpio)
{
    // consider why must malloc be used to allocate device
    ps2_device_t *dev = malloc(sizeof(*dev));
	gpio_interrupts_init();

    dev->clock = clock_gpio;
	dev->rb = rb_new(); // initialize ringbuffer queue
	dev->calls = -2; // start at -2, so actual bits for scancode read when calls is 0.
	dev->number = 0; // tracks scancode
	dev->parity = 0; // tracks parity
	dev->bit = 0;
    gpio_set_input(dev->clock); // configure clock line
    gpio_set_pullup(dev->clock);

    dev->data = data_gpio;
    gpio_set_input(dev->data); // configure data line
    gpio_set_pullup(dev->data);

	gpio_enable_event_detection(dev->clock, GPIO_DETECT_FALLING_EDGE);
    gpio_interrupts_register_handler(dev->clock, handle_ps2, dev);
    gpio_interrupts_enable();

    return dev;
}

/* This function in an interrupts handler function that collects bits for 
 * a scancode by reading from the data line only if a falling edge is detected
 * from the clock line. The function checks for error correcting codes by
 * verifying the start, stop, and parity bits are correct. A ps2_device_t 
 * struct pointer is passed in as auxilary data.
 */
static void handle_ps2(unsigned int pc, void *aux_data){
	ps2_device_t* dev = (ps2_device_t *) aux_data;
    // check for clock falling edge
    if (gpio_check_and_clear_event(dev->clock)) {
		dev->calls += 1;
    }
	// if no falling edge, read again
	else {
		return;
	}
    if (dev->calls == -1) {
		// This ensures that the start bit is correctly
    	// a 0.
		if (gpio_read(dev->data) != 0) {
		     dev->calls = -2;
		}
    }
    // Reads the next 10 bits after start bit
	// dev->calls should be 0 and above here
    else if (dev->calls >= 0) {
		// Next 8 bits received are the bits for data we want to use
		if (dev->calls < 8) {
		    dev->bit = (gpio_read(dev->data) ? 1 : 0);
		    dev->parity += dev->bit;
			// LSB first received
		    dev->number += dev->bit << dev->calls; 
		}
		// The 10th bit received is the parity bit
		else if (dev->calls == 8) {
		    dev->parity += (gpio_read(dev->data) ? 1 : 0);
		    // Total number of ones must be odd, if it isn't
		    // discard the bits and start reading again
		    if (dev->parity % 2 == 0) {
				dev->calls = -2;
				dev->number = 0;
				dev->parity = 0;
				dev->bit = 0;
		    }
		}
		// The 11th bit recieved must be the stop bit and must be 1
		// If it is not, loop starts over and data should be read again
		else if (dev->calls == 9) {
		    if (gpio_read(dev->data) == 1) {
				rb_enqueue(dev->rb, dev->number);
		    }
 		    dev->calls = -2;
		    dev->number = 0;
		    dev->parity = 0;
		    dev->bit = 0;
		}
    }
	aux_data = dev;
}

// This function reads a single ps2 scan code. It always returns a correctly received scan code, if there
// is one queued up.
unsigned char ps2_read(ps2_device_t *dev)
{
    while (rb_empty(dev->rb)) {/* spin */}
    unsigned int temp;
	rb_dequeue(dev->rb, (int *) &temp);
    return temp;
}

