// File: testing.c
// ---------------
// Unit testing example

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

// The count_bits function is intended to return the count of
// on bits in a numeric value. But oops-- the code as
// written below is buggy!  A program this simple we could try to
// validate "by inspection" but instead we will demonstrate
// how to find the problem using rudimentary unit tests and
// the simple assert macro defined above.
//
// You assert a condition that must be true and if the expression
// evaluates to false, assert treats as an error.
// We use the red and greeen LEDs on the Pi board as status indicators.
// When an assert fails, it calls abort(), which stops here
// and flashes the red LED. If the program successfully finishes
// executing (no assert failed), the green LED turns on.
// Thus the flashing red LED of doom tells you that your
// program is failing at least one test.
// The solid green light means your program passed the tests you
// asked it to check.

int count_bits(unsigned int val)
{
    int count = 0;

    while (val != 0) {
        val = val >> 1;   // BUGGY: should test/count lsb first, then shift
        if (val & 1)      // which inputs are affected by this bug?
            count++;
    }
    return count;
}


void main(void)
{
    assert(count_bits(0) == 0);
    assert(count_bits(8) == 1);
    assert(count_bits(6) == 2);
    assert(count_bits(7) == 3);
    assert(count_bits(0xffffffff) == 32);
    assert(count_bits(5) == 1);   // this test case is misconstructed!

	// read cstart.c to find out what happens after main() finishes
}
