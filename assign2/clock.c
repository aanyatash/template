#include "gpio.h"
#include "timer.h"


static unsigned char digits[16] = { 0b00111111,   // 0
				    0b00110000,   // 1
            			    0b01011011,   // 2
				    0b01001111,   // 3
				    0b01100110,   // 4
				    0b01101101,   // 5
				    0b00000111,   // 6
				    0b00000111,   // 7
				    0b01111111,   // 8
				    0b01101111,   // 9
				    0b01110111,   // A (10)
				    0b01111100,   // B (11)
				    0b00111001,   // C (12)
				    0b01011110,   // D (13)
				    0b01111001,   // E (14)
				    0b01110001 }; // F (15)




static unsigned int segment[8] = {GPIO_PIN26,  GPIO_PIN19, GPIO_PIN13, GPIO_PIN6,
                           GPIO_PIN5, GPIO_PIN11, GPIO_PIN9, GPIO_PIN10};
static unsigned int digit[4] = {GPIO_PIN21, GPIO_PIN20, GPIO_PIN16, GPIO_PIN12};
static unsigned int button = GPIO_PIN2;




static display_single_digit();
static configure();
static undisplay_single_digit();

void main(void)
{
    configure();
    gpio_write( digit[0], 1);
    for (int i = 0; i < 16; i ++) {
	display_single_digit( digits[i] );
        timer_delay_ms(200);
        undisplay_single_digit();
    }
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

void display_single_digit( unsigned char digit_display ) {
    unsigned char LSB;
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


void undisplay_single_digit() {
    for (int i = 0; i < 8; i++) {
	gpio_write( segment[i] , 0);
    }
}
