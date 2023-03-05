#include "gpio.h"
#include "keyboard.h"
#include "printf.h"
#include "timer.h"
#include "uart.h"
#include "gpio_interrupts.h"
#include "interrupts.h"

/*
 * This function tests the behavior of the assign5 ps2
 * implementation versus the new-improved assign7 version. If using the
 * assign5 implementation, a scancode that arrives while the main program
 * is waiting in delay is simply dropped. Once you upgrade your
 * ps2 module to be interrupt-driven, those scancodes should
 * be queued up and can be read after delay finishes.
 */
static void test_read_delay(void)
{
    while (1) {
        printf("Test program waiting for you to press a key (q to quit): ");
        uart_flush();
        char ch = keyboard_read_next();
        printf("\nRead: %c\n", ch);
        if (ch == 'q') break;
        printf("Test program will now pause for 1 second... ");
        uart_flush();
        timer_delay(1);
        printf("done.\n");
    }
    printf("\nGoodbye!\n");
}


// This function doesn't test anything, it just waits in a delay
// loop, keeping the processor busy. This allows to observe whether your
// interrupt is being triggered to insert itself.
void check_interrupts_received(void)
{
    printf("Type on your PS/2 keyboard to generate events. You've got 10 seconds, go!\n");
    timer_delay(10);
    printf("Time's up!\n");
}


void main(void)
{
    gpio_init();
    timer_init();
    uart_init();
    interrupts_init();

    keyboard_init(KEYBOARD_CLOCK, KEYBOARD_DATA);

	interrupts_global_enable();

    //check_interrupts_received();


    test_read_delay();
    uart_putchar(EOT);
}
