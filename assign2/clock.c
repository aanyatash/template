/* Name: Aanya Tashfeen
 * Filename: clock.c
 * CS 107E Assignment 2 Extension
 * This program allows time to be set on a clock by clicking two buttons and 
 * starts the clock by holding the left button down. 
 * The clock resets to 0000 in set mode by holding the right button down. 
 * The clock counts up until 99:59.
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
static unsigned int button[2] = {GPIO_PIN2, GPIO_PIN3};

static void configure();
static void button_to_start();
static void display_single_digit( unsigned char digit_display, unsigned char digit_num );
static void display_four_digits( unsigned char digit_1, unsigned char digit_2, unsigned char digit_3, unsigned char digit_4 );
static void start(unsigned int digit1_id, unsigned int digit2_id, unsigned int digit3_id, unsigned int digit4_id );
static void flash_display(unsigned int digit1_id, unsigned int digit2_id, unsigned int digit3_id, unsigned int digit4_id);
static void debouncing();
static unsigned int long_button_press();
static void set(unsigned int digit1_id, unsigned int digit2_id, unsigned int digit3_id, unsigned int digit4_id);

void main(void)
{
    configure();
    button_to_start(); // Must click button to start/go into set mode
    set(0, 0, 0, 0);
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

    gpio_set_input(button[0]); // configure buttons
    gpio_set_input(button[1]);
}


/* This function displays the ---- start display and only exits the
 * function once the button is clicked. It essentially allows the button
 * to function as a start button.
 */
void button_to_start() {
    while (1) {
        display_four_digits( digits[16], digits[16], digits[16], digits[16] );
    	if (gpio_read(button[0]) == 0) return;
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

/* This function debounces the button to prevent any spurious reads after a button press.
 * The function takes 4 arguments that are unsigned integers which describe the current time.
 * This allows current time to be displayed continuously while debouncing occurs.
 */
void debouncing(unsigned int digit1_id, unsigned int digit2_id, unsigned int digit3_id, unsigned int digit4_id ) {
    unsigned int debounce_delay = 500*1000; // time delay in us
    unsigned int time_pressed = timer_get_ticks();
    while (timer_get_ticks() - time_pressed < debounce_delay) { // While loop prevents button reads
	display_four_digits(digits[digit1_id], digits[digit2_id], digits[digit3_id], digits[digit4_id]);
    }
}

/* This function allows a long button to be detected. The function takes 5 arguments that are unsigned 
 * integers. The first 4  describe the current time, allowing current time to be displayed while in this function.
 * The fifth parameter is which button to check long press for.
 * This function returns an unsigned integer 1 or 0 which describes true or false, in terms of whether
 * it was a long button press or not.
 */
unsigned int long_button_press(unsigned int digit1_id, unsigned int digit2_id, unsigned int digit3_id, unsigned int digit4_id, unsigned int button_id) {
    unsigned int start_press = timer_get_ticks(); // Start timer
    debouncing(digit1_id, digit2_id, digit3_id, digit4_id); // Debounces button, so no spurious reads
    unsigned int long_time_press = 2000*1000; // time in microseconds for 2 seconds
    unsigned int long_pressed = 1;
    unsigned int released;
    if (gpio_read(button[button_id]) == 1) {
	released = timer_get_ticks(); // End timer when button released
    	if (released - start_press < long_time_press) {
		long_pressed = 0; // Not long pressed, so store and return 0
    	}
    }
    return long_pressed;
}


/* This function starts the timer from whatever set time was. The clock counts until 99:59 and
 * then restarts to 00 00. This functions uses a timer.c module called timer_get_ticks()
 * which uses the Pi's internal system timer. The function takes 4 unsigned integers as
 * arguments and these describe the set starting time.
 */
void start(unsigned int digit1_id, unsigned int digit2_id, unsigned int digit3_id, unsigned int digit4_id ) {
    unsigned int time_start;
    while (gpio_read(button[0]) == 0) { /* spin */ } // Accounts for very long button holding
    while (1) {
	    for (int i = digit1_id; i < 10; i++) {   // controls digit 1 to max at 9
		for (int j = digit2_id; j < 10; j++) {  // controls digit 2 to max at 9
			for (int k = digit3_id; k < 6; k++) {  // controls digit 3 to max at 6 as seconds timer can't exceed 60
				for (int l = digit4_id; l < 10; l++) {  // controls digit 4 to max at 9
					time_start = timer_get_ticks(); // uses Pi's system timer to count time
					while ((timer_get_ticks() - time_start) < 1000*1000) {
						// Only changes displayed digit when timer counts 1 second
						display_four_digits(digits[i], digits[j], digits[k], digits[l]);
						// Returns to set mode is left button pressed
						if (gpio_read(button[0]) == 0) {
							set(i, j, k, l);
						}
					}
				}
				digit4_id = 0; // Resets to starting value of 0
			}
			digit3_id = 0;
		}
		digit2_id = 0;
	   }
	   digit1_id = 0;
    }
}


/* This function takes and input of 4 unsigned integers that describe the digits displayed in each segement.
 * The function will then flash these digits three times. This indicates the set mode.
 */
void flash_display(unsigned int digit1_id, unsigned int digit2_id, unsigned int digit3_id, unsigned int digit4_id) {
   for (int i = 0; i < 3; i++) { // Flash three times
	for (int j = 0; j < 4; j++) {
	     gpio_write(digit[j], 0); // Turn all digits on display off
        }
	timer_delay_ms(500);
	unsigned int start = timer_get_ticks();
	while (timer_get_ticks() - start < 500*1000) { // Turn all digits on for 500 ms
        	display_four_digits(digits[digit1_id], digits[digit2_id], digits[digit3_id], digits[digit4_id]);
	}	
    }

}


/* This function is essentially set mode. The function allows minutes and seconds to be set through button
 * clicks going up and it sets it from the time the clock is stopped. To exit set mode and go into start mode,
 * the user must click the left button for at least 2 seconds.
 */
void set(unsigned int digit1_id, unsigned int digit2_id, unsigned int digit3_id, unsigned int digit4_id) {
    flash_display(digit1_id, digit2_id, digit3_id, digit4_id); // Flashes the current time
    // stores current time in an array to keep track of time increments
    unsigned int cur_time[4] = {digit1_id, digit2_id, digit3_id, digit4_id};

    // In set mode infinitely, until start mode triggered
    while (1) {

        display_four_digits(digits[cur_time[0]], digits[cur_time[1]], digits[cur_time[2]], digits[cur_time[3]]);

	if ( gpio_read(button[1]) == 0 ) { // If SECONDS button pressed
	    // SECONDS button also reset to 0000 if pressed long enough
	    if (long_button_press(cur_time[0], cur_time[1], cur_time[2], cur_time[3], 1)) { 
	        while (gpio_read(button[1]) == 0) { /* spin */ } // Accounts for very long button holding
		cur_time[0] = 0;
		cur_time[1] = 0;
		cur_time[2] = 0;
		cur_time[3] = 0;
	 	continue;
	    }
	    if (cur_time[0] == 9 && cur_time[1] == 9 && cur_time[2] == 5 && cur_time[3] == 9) { // If current time is 99 59
		// Change time to 0000
		cur_time[0] = 0;
		cur_time[1] = 0;
		cur_time[2] = 0;
		cur_time[3] = 0;
	    }
	    else if (cur_time[1] == 9 && cur_time[2] == 5 && cur_time[3] == 9) { // Otherwise, if current time is x9 59
		// Change time to x+1 000
		cur_time[0] += 1;
		cur_time[1] = 0;
		cur_time[2] = 0;
		cur_time[3] = 0;
	    }
	    else if (cur_time[2] == 5 && cur_time[3] == 9) { // Otherwise, if current time is xy 59
		// Change time to x y+1 00
		cur_time[1] += 1;
		cur_time[2] = 0;
		cur_time[3] = 0;
	    }
	    else if (cur_time[3] == 9) { // Otherwise, if current time is xy z9
		// Change time to xy z+1 0
		cur_time[2] += 1;
		cur_time[3] = 0;
	    }
	    else { // Else, just change last seconds digit
		cur_time[3] += 1;
	    }
        display_four_digits( digits[cur_time[0]], digits[cur_time[1]], digits[cur_time[2]],digits[cur_time[3]] );
	debouncing(cur_time[0], cur_time[1], cur_time[2], cur_time[3]);
	continue;
	}


        if ( gpio_read(button[0]) == 0 ) { // If MINUTES button pressed
	    // MINUTES button also start mode button if pressed long enough
	    if (long_button_press(cur_time[0], cur_time[1], cur_time[2], cur_time[3], 0)) { 
	    	start(cur_time[0], cur_time[1], cur_time[2], cur_time[3]);
	 	continue;
	    }
	    if (cur_time[0] == 9 && cur_time[1] == 9) { // If current time is 99 xy
		// Change time to 0000
		cur_time[0] = 0;
		cur_time[1] = 0;
		cur_time[2] = 0;
		cur_time[3] = 0;
	    }
	    else if (cur_time[1] == 9) { // Otherwise, ff current time is x9 yz
		// Change time to x+1 0 yz
		cur_time[0] += 1;
		cur_time[1] = 0;
	    }
	    else { // Else, just change last minutes digit
		cur_time[1] += 1;
	    }
        display_four_digits( digits[cur_time[0]], digits[cur_time[1]], digits[cur_time[2]], digits[cur_time[3]]);
	debouncing(cur_time[0], cur_time[1], cur_time[2], cur_time[3]);
	continue;
        }
    }
}
