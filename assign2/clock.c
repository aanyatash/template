/* Name: Aanya Tashfeen
 * Filename: clock.c
 * CS 107E Assignment 2
 * This program starts a clock by clicking a button and then 
 * counts up until 99:59
 */

#include "gpio.h"
#include "timer.h"

// The binary values where 1 is the segment that should be lit for each digit
// to be displayed
static unsigned char digits[17] = { 0b00111111,   // 0
				    0b00110000,   // 1
            			    0b01011011,   // 2
				    0b01001111,   // 3
				    0b01100110,   // 4
				    0b01101101,   // 5
				    0b01111101,   // 6
				    0b00000111,   // 7
				    0b01111111,   // 8
				    0b01101111,   // 9
				    0b01110111,   // A (10)
				    0b01111100,   // b (11)
				    0b00111001,   // C (12)
				    0b01011110,   // d (13)
				    0b01111001,   // E (14)
				    0b01110001,   // F (15)
				    0b01000000 }; // - (16)



static unsigned int segment[8] = {GPIO_PIN26,  GPIO_PIN19, GPIO_PIN13, GPIO_PIN6,
                           GPIO_PIN5, GPIO_PIN11, GPIO_PIN9, GPIO_PIN10};
static unsigned int digit[4] = {GPIO_PIN21, GPIO_PIN20, GPIO_PIN16, GPIO_PIN12};
static unsigned int button = GPIO_PIN2;

static void configure();
static void button_to_start();
static void display_single_digit( unsigned char digit_display, unsigned char digit_num );
static void display_four_digits( unsigned char digit_1, unsigned char digit_2, unsigned char digit_3, unsigned char digit_4 );
static void start();

void main(void)
{
    configure();
    button_to_start();
    start(); 
}

/* This function sets all the GPIO pins for the clock display to output
 * and the GPIO pin for the button to input.
 */
void configure() {
    gpio_init();
    for (int i = 0; i < 8; i++) {  // configure segments
        gpio_set_output(segment[i]);
    }

    for (int i = 0; i < 4; i++) {  // configure digits
        gpio_set_output(digit[i]);
    }

    gpio_set_input(button); // configure button
}


/* This function displays the ---- start display and only exits the
 * function once the button is clicked. It essentially allows the button
 * to function as a start button.
 */
void button_to_start() {
    while (1) {
        display_four_digits( digits[16], digits[16], digits[16], digits[16] );
    	if (gpio_read(button) == 0) return;
    }
}

/* This function displays one digit. It iterates through every bit for a digit
 * in the digits array to determine which segments should be lit up. It takes two
 * arguments - digit_display is the binary value describing the segments to be lit up
 * for the digit needing to be displayed and digit_num describes  which digit on the clock 
 * needs to be lit up.
 */
void display_single_digit( unsigned char digit_display, unsigned char digit_num ) {
    unsigned char LSB;
    gpio_write( digit[digit_num], 1 );  // turns digit on

    // Iterates through every bit in the binary value which corresponds to every segment
    for (int i = 0; i < 8; i++) {
	LSB = digit_display & 1; // Starts at least significant bit
	gpio_write( segment[i] , LSB); // Writes 0 or 1 to the pin based on whether it needs to be lit
	digit_display >>= 1; // Moves onto next bit
    }     
}

/* This function takes 4 arguments which each describe which digit needs to be displayed
 * in the first, second, third, and fourth digits of the clock display. The arguments are
 * 1 byte unsigned chars since the max digit is 9. It turns each digit on and off really
 * quickly to create the illusions that they're all on.
 */
void display_four_digits( unsigned char digit_1, unsigned char  digit_2, unsigned char digit_3, unsigned char digit_4 ) {
    unsigned char to_display[4] = { digit_1, digit_2, digit_3, digit_4 };
    // Iterates through each of the four digit to turn them on and off very quickly
    for (int i = 0; i < 4; i++ ) {
 	display_single_digit( to_display[i], i ); // Displays one digit
    	timer_delay_us(2500);
    	gpio_write( digit[i] , 0 ); // Turns one digit off
    }
}

/* This function starts the timer from 00 00. The clock counts until 99:59 and
 * then restarts to 00 00. This functions uses a timer.c module called timer_get_ticks()
 * which uses the Pi's internal system timer.
 */
void start() {
    unsigned int time_start;
    while (1) {
	    for (int i = 0; i < 10; i++) {   // controls digit 1 to max at 9
		for (int j = 0; j < 10; j++) {  // controls digit 2 to max at 9
			for (int k = 0; k < 6; k++) {  // controls digit 3 to max at 6 as seconds timer can't exceed 60
				for (int l = 0; l < 10; l++) {  // controls digit 4 to max at 9
					time_start =  timer_get_ticks(); // uses Pi's system timer to count time
					while ((timer_get_ticks() - time_start) < 1000*1000) {
						// Only changes displayed digit when timer counts 1 second
						display_four_digits(digits[i], digits[j], digits[k], digits[l]);
						
					}
				}
			}
		}
	    }
    }
}
