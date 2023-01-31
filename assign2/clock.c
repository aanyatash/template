#include "gpio.h"
#include "timer.h"


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
				    0b01111100,   // B (11)
				    0b00111001,   // C (12)
				    0b01011110,   // D (13)
				    0b01111001,   // E (14)
				    0b01110001    // F (15
				    0b01000000 }; // - (16)




static unsigned int segment[8] = {GPIO_PIN26,  GPIO_PIN19, GPIO_PIN13, GPIO_PIN6,
                           GPIO_PIN5, GPIO_PIN11, GPIO_PIN9, GPIO_PIN10};
static unsigned int digit[4] = {GPIO_PIN21, GPIO_PIN20, GPIO_PIN16, GPIO_PIN12};
static unsigned int button = GPIO_PIN2;



static void configure();
static void display_single_digit( unsigned char digit_display, unsigned char digit_num );
//static void undisplay_single_digit();
static void display_four_digits( unsigned char digit_1, unsigned char digit_2, unsigned char digit_3, unsigned char digit_4 );

void main(void)
{
    configure(); 
//    for (int i = 0; i < 16; i = i+4) {
//	display_single_digit( digits[i], 1 );
//        timer_delay_ms(1000);
//        undisplay_single_digit();


// waits for button click
// display ----  (digits[16])
// click button
// call start function:
// set start time with timer get ticks
// four for loops -- two outer to 9, third to 6, fourth to 9
// while (timer get ticks - start) <  100 {display 0000 which is digits[0] for all}
// increment digit_4 by 1 to give digits[1]


// add some for loop logic to increment each number
// max is 99 59

    display_four_digits(digits[0], digits[1], digits[2], digits[3] );
}


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

void display_single_digit( unsigned char digit_display, unsigned char digit_num ) {
    unsigned char LSB;
    gpio_write( digit[digit_num], 1 );  // turns digit on
    for (int i = 0; i < 8; i++) {
	LSB = digit_display & 1;
	gpio_write( segment[i] , LSB);
	digit_display >>= 1;
    }     

    // read LSB
    // light pin ( SET )
    // right shift
    // do this 8 times
}


//void undisplay_single_digit() {  // NOT NEEDED
//    for (int i = 0; i < 8; i++) {
//	gpio_write( segment[i] , 0);
//    }
//}
//

void display_four_digits( unsigned char digit_1, unsigned char  digit_2, unsigned char digit_3, unsigned char digit_4 ) {
    unsigned char to_display[4] = { digit_1, digit_2, digit_3, digit_4 };
    while (1) {
	for (int i = 0; i < 4; i++ ) {
    	    display_single_digit( to_display[i], i );
    	    timer_delay_us(2500);
    	    gpio_write( digit[i] , 0 ); // turns one digit off
	}
    }
}
