/* Name: Aanya Tashfeen
 * Filename: test_gpio_timer.c
 * This file contains tests for the full clock breadboard, the gpio module, and
 * the timer module.
 */


#include "gpio.h"
#include "timer.h"

// You call assert on an expression that you expect to be true. If expr
// actually evaluates to false, then assert calls abort, which stops
// your program and flashes the red light of doom.
#define assert(expr) if(!(expr)) abort()

// infinite loop that flashes the red power LED (GPIO 35)
void abort(void) {
    volatile unsigned int *FSEL3 = (void *)0x2020000c;
    volatile unsigned int *SET1 = (void *)0x20200020;
    volatile unsigned int *CLR1 = (void *)0x2020002c;
    volatile int delay;  // volatile counter to prevent optimize out of delay loop
    int bit35 =  1 << 3;

    // Configure GPIO 35 function to be output.
    // This wipes functions for other gpios in this register (30-39)
    // but that's okay, because this is a dead-end routine.
    *FSEL3 = 1 << 15;
    while (1) { // infinite loop
        *SET1 = bit35;  // on
        for (delay = 0x100000; delay > 0; delay--) ;
        *CLR1 = bit35;  // off
        for (delay = 0x100000; delay > 0; delay--) ;
    }
}

void test_gpio_set_get_function(void) {
    gpio_init();

    // Test get pin function (pin 2 defaults to input)
    assert( gpio_get_function(GPIO_PIN2) == GPIO_FUNC_INPUT );

    // Set pin 2 to output, confirm get returns what was set
    gpio_set_output(GPIO_PIN2);
    assert( gpio_get_function(GPIO_PIN2) == GPIO_FUNC_OUTPUT );

    // Set pin 2 back to input, confirm get returns what was set
    gpio_set_input(GPIO_PIN2);
    assert( gpio_get_function(GPIO_PIN2) == GPIO_FUNC_INPUT );
    
    // Set pin 3 to input, confirm get returns what was set and whether pin 2 still set
    gpio_set_input(GPIO_PIN3);
    assert( gpio_get_function(GPIO_PIN2) == GPIO_FUNC_INPUT );
    assert( gpio_get_function(GPIO_PIN3) == GPIO_FUNC_INPUT );

    unsigned int segment[8] = {GPIO_PIN26,  GPIO_PIN19, GPIO_PIN13, GPIO_PIN6,
                               GPIO_PIN5, GPIO_PIN11, GPIO_PIN9, GPIO_PIN10};
    unsigned int digit[4] = {GPIO_PIN21, GPIO_PIN20, GPIO_PIN16, GPIO_PIN12};

    for (int i = 0; i < 8; i++) {  // configure segments and check that functions being preserved
        gpio_set_output(segment[i]);
        for (int k = 0; k < i; k++) {
         assert (gpio_get_function(segment[k]) == GPIO_FUNC_OUTPUT);
        }
    }
    for (int i = 0; i < 4; i++) {  // configure digits and check that functions being preserved
        gpio_set_output(digit[i]);
        for (int k = 0; k < i; k++) {
        	assert (gpio_get_function(digit[k]) == GPIO_FUNC_OUTPUT);
        }
    }
       
    }

void test_gpio_read_write(void) {
    gpio_init();
    // set pin 20 to output before gpio_write
    gpio_set_function(GPIO_PIN20, GPIO_FUNC_OUTPUT);

    // gpio_write low, confirm gpio_read reads what was written
    gpio_write(GPIO_PIN20, 0);
    assert( gpio_read(GPIO_PIN20) ==  0 );

    // gpio_write high, confirm gpio_read reads what was written
    gpio_write(GPIO_PIN20, 1);
    assert( gpio_read(GPIO_PIN20) ==  1 );

    // gpio_write low, confirm gpio_read reads what was written
    gpio_write(GPIO_PIN20, 0);
    assert( gpio_read(GPIO_PIN20) ==  0 );
    
    // set pin 21 to output before gpio write
    gpio_set_function(GPIO_PIN21, GPIO_FUNC_OUTPUT);

    // gpio_write high, confirm what was written and pin 20 still low
    gpio_write(GPIO_PIN21, 1);
    assert( gpio_read(GPIO_PIN21) == 1);
    assert( gpio_read(GPIO_PIN20) == 0);

}

void test_invalid(void) {
	gpio_set_function( -1, GPIO_FUNC_OUTPUT);
        assert( gpio_get_function( -1 )  == GPIO_INVALID_REQUEST );
 
	gpio_set_function( 55, GPIO_FUNC_OUTPUT);
        assert( gpio_get_function( 55 ) == GPIO_INVALID_REQUEST );

	gpio_write(-1, 1);
        assert( gpio_read( -1) == GPIO_INVALID_REQUEST );


	gpio_set_function( 2, GPIO_FUNC_OUTPUT);
	gpio_write(2, 3);
        assert( gpio_read(2) == 0);

}

void test_all_gpio_read_write(void) {
    unsigned int segment[8] = {GPIO_PIN26,  GPIO_PIN19, GPIO_PIN13, GPIO_PIN6,
                               GPIO_PIN5, GPIO_PIN11, GPIO_PIN9, GPIO_PIN10};
    unsigned int digit[4] = {GPIO_PIN21, GPIO_PIN20, GPIO_PIN16, GPIO_PIN12};
    for (int i = 0; i < 8; i++) {  // configure segments
        gpio_set_output(segment[i]);
    }

    for (int i = 0; i < 4; i++) {  // configure digits
        gpio_set_output(digit[i]);
    }

    while (1) {
        for (int i = 0; i < 4; i++) {   // iterate over digits
            gpio_write(digit[i], 1);    // turn on digit
	    assert( gpio_read(digit[i]) == 1);
            for (int j = 0; j < 8; j++) {   // iterate over segments
                gpio_write(segment[j], 1);  // turn on segment
		assert( gpio_read(segment[j]) == 1);
                timer_delay_ms(200);
                gpio_write(segment[j],  0);  // turn off segment
 		assert( gpio_read(segment[j]) == 0 );
            }
            gpio_write(digit[i], 0);    // turn off digit
            assert( gpio_read(digit[i]) == 0 );
        }
    }

}

void test_timer(void) {
    timer_init();

    // Test timer tick count incrementing
    unsigned int start = timer_get_ticks();
    for( int i=0; i<10; i++ ) { /* Spin */ }
    unsigned int finish = timer_get_ticks();
    assert( finish > start );

    // Test timer delay
    int usecs = 100;
    start = timer_get_ticks();
    timer_delay_us(usecs);
    finish = timer_get_ticks();
    assert( finish >= start + usecs );

    // Test msec timer delay
    int msecs = 250;
    start = timer_get_ticks();
    timer_delay_ms(250);
    finish = timer_get_ticks();
    assert( finish >= start + 1000*msecs ); 

    // Test sec timer delay
    int secs = 3;
    start = timer_get_ticks();
    timer_delay(3);
    finish = timer_get_ticks();
    assert( finish >= start + 1000*1000*secs ); 


}

/* This function tests the clock set up on a breadboard and verifies the
 * button works. 
 */
void test_breadboard(void) {
    unsigned int segment[8] = {GPIO_PIN26,  GPIO_PIN19, GPIO_PIN13, GPIO_PIN6,
                               GPIO_PIN5, GPIO_PIN11, GPIO_PIN9, GPIO_PIN10};
    unsigned int digit[4] = {GPIO_PIN21, GPIO_PIN20, GPIO_PIN16, GPIO_PIN12};
    unsigned int button = GPIO_PIN2;

    gpio_init();
    for (int i = 0; i < 8; i++) {  // configure segments
        gpio_set_output(segment[i]);
    }

    for (int i = 0; i < 4; i++) {  // configure digits
        gpio_set_output(digit[i]);
    }
 
    gpio_set_input(button); // configure button
    
    while (1) { // loop forever (finish via button press, see below)
        for (int i = 0; i < 4; i++) {   // iterate over digits
            gpio_write(digit[i], 1);    // turn on digit
            for (int j = 0; j < 8; j++) {   // iterate over segments
                if (gpio_read(button) == 0) return; // read button, exit if button pressed
                gpio_write(segment[j], 1);  // turn on segment
                timer_delay_ms(200);
                gpio_write(segment[j], 0);  // turn off segment
            }
           gpio_write(digit[i], 0);    // turn off digit
        }
    }
}

// Uncomment each call below when you have implemented the functions
// and are ready to test them

void main(void) {
   test_gpio_set_get_function();
   test_gpio_read_write();
   test_timer();
  // test_breadboard();
   test_invalid();
}
