/* Name: Aanya Tashfeen
 * Filename: timer.c
 * This file contains a timer module that makes use of the Pi's system
 * timer.
 */

#include "timer.h"

static volatile unsigned int *TIMER = (unsigned int*) 0x20003004;

void timer_init(void) {
}

/* This function returns the system tick count where
 * one tick is equivalent to one microsecond.
 */
unsigned int timer_get_ticks(void) {
    return *TIMER;
 }

/* This function delays the program for an inputted
 * number of microseconds which is specified as an unsigned
 * integer.
 */
void timer_delay_us(unsigned int usecs) {
    unsigned int start = timer_get_ticks();
    while (timer_get_ticks() - start < usecs) { /* spin */ }
}

/* This function delays the program for an inputted
 * number of milliseconds which is specified as an unsigned
 * integer.
 */
void timer_delay_ms(unsigned int msecs) {
    timer_delay_us(1000*msecs);
}

/* This function delays the program for an inputted
 * number of seconds which is specified as an unsigned
 * integer.
 */
void timer_delay(unsigned int secs) {
    timer_delay_us(1000000*secs);
}
