#include "assert.h"
#include "keyboard.h"
#include "printf.h"
#include "shell.h"
#include "strings.h"
#include "uart.h"
#include "timer.h"

#define ESC_SCANCODE 0x76

static void test_keyboard_scancodes(void)
{
    printf("\nNow reading single scancodes. Type ESC to finish this test.\n");
    while (1) {
        unsigned char scancode = keyboard_read_scancode();
        printf("[%02x]\n", scancode);
        if (scancode == ESC_SCANCODE) break;
    }

	/* I confirmed that it was printing out for a lot of letters, the function keys,
	 * and the punctuation keys. I also tested holding down one key and holding down multiple
	 * keys.
	 */

    printf("\nDone with scancode test.\n");
}

static void test_keyboard_sequences(void)
{
    printf("\nNow reading scancode sequences (key actions). Type ESC to finish this test.\n");
    while (1) {
        key_action_t action = keyboard_read_sequence();
        printf("%s [%02x]\n", action.what == KEY_PRESS ? "  Press" :"Release", action.keycode);
        if (action.keycode == ESC_SCANCODE) break;
    }

	/* I tested pressing down on a key. All versions of alt and control were also tested to
	 * confirm that extended key presses and releases were working as expected.
	 */
    printf("Done with scancode sequences test.\n");
}

static void test_keyboard_events(void)
{
    printf("\nNow reading key events. Type ESC to finish this test.\n");
    while (1) {
        key_event_t evt = keyboard_read_event();
        printf("%s PS2_key: {%c,%c} Modifiers: 0x%x\n", evt.action.what == KEY_PRESS? "  Press" : "Release", evt.key.ch, evt.key.other_ch, evt.modifiers);
        if (evt.action.keycode == ESC_SCANCODE) break;
    }
    printf("Done with key events test.\n");

	/* When testing keyboard events, I pressed every modifier and confirmed the correct
	 * changes reflected in the modifier value. I also pressed multipla/all of them at once,
	 * and confirmed the correct modifier value. I tested many of the letter and punctuation keys
	 * and also the function keys and the whitespace and backspace keys.
	 */
}

static void test_keyboard_chars(void)
{
    printf("\nNow reading chars. Type ESC to finish this test.\n");
    while (1) {
        char c = keyboard_read_next();
        if (c >= '\t' && c <= 0x80)
            printf("%c", c);
        else
            printf("[%02x]", c);
        if (c == ps2_keys[ESC_SCANCODE].ch) break;
    }

	/* When testing keyboard chars, I tested that my shift and caps lock produced the correct
	 * output for letter and non-letter keys. I also tested that control and alt did nothing.
	 * I also tested shift and caps lock in unison to ensure that shift always dominated. I tested
	 * all the modifier keys in unison as well, to ensure that shift still dominated and others
	 * did nothing. I tested the special function keys, f1 to f12, to ensure a hex value was 
	 * returned and I tested with whitespace and backspace keys and they behaved as expected.
	 * The escape key alos behaved as expected, returning 95 and then exiting the program.
	 * I tested holding down modifer keys and changing the letters and holding down letter keys.
	 */

    printf("\nDone with key chars test.\n");
}

static void test_keyboard_assert(void)
{
    char ch;
    printf("\nHold down Shift and type 'g'\n");
    ch = keyboard_read_next();
    assert(ch == 'G');  // confirm user can follow directions and correct key char generated
}

static void test_shell_evaluate(void)
{
    shell_init(keyboard_read_next, printf);

    printf("\nTest shell_evaluate on fixed commands.\n");

    // Test echo
    int ret = shell_evaluate("echo hello, world!");
    printf("Command result is zero if successful, is it? %d\n", ret);

    // Test help cmd
	ret = shell_evaluate("help echo");
    printf("Command result is zero if successful, is it? %d\n", ret);

    // Test help
	ret = shell_evaluate("help");
    printf("Command result is zero if successful, is it? %d\n", ret);

    // Test help with trailing space
	ret = shell_evaluate("help ");
    printf("Command result is zero if successful, is it? %d\n", ret);

    // Test leading space
	ret = shell_evaluate("	help");
    printf("Command result is zero if successful, is it? %d\n", ret);

    // Test invalid command
	ret = shell_evaluate("help hi");
    printf("Command result is zero if successful, is it? %d\n", ret);

    // Test more than one command
	ret = shell_evaluate("help echo help");
    printf("Command result is zero if successful, is it? %d\n", ret);

    // Test peek example given in spec
	ret = shell_evaluate("peek 0x8000");
    printf("Command result is zero if successful, is it? %d\n", ret);

    // Test peek with no arguments
	ret = shell_evaluate("peek");
    printf("Command result is zero if successful, is it? %d\n", ret);

    // Peek with invalid address
	ret = shell_evaluate("peek bob");
    printf("Command result is zero if successful, is it? %d\n", ret);

    // Peek, not 4-byte aligned
	ret = shell_evaluate("peek 7");
    printf("Command result is zero if successful, is it? %d\n", ret);

    // Test poke at start address, use peek to confirm successful poke
	ret = shell_evaluate("poke 0x8000 1");
    printf("Command result is zero if successful, is it? %d\n", ret);
	ret = shell_evaluate("peek 0x8000");
    printf("Command result is zero if successful, is it? %d\n", ret);
 
    // Change the value at previous poke to 0 - shouldn't work as 0 is invalid
	// Use peek to confirm failure
	ret = shell_evaluate("poke 0x8000 0");
    printf("Command result is zero if successful, is it? %d\n", ret);
	ret = shell_evaluate("peek 0x8000");
    printf("Command result is zero if successful, is it? %d\n", ret);

    // Test not enough arguments for poke
	ret = shell_evaluate("poke 0x8000");
    printf("Command result is zero if successful, is it? %d\n", ret);

    // Invalid value for poke
	ret = shell_evaluate("poke 0x8000 wilma");
    printf("Command result is zero if successful, is it? %d\n", ret);

    // Invalid adress for poke
	ret = shell_evaluate("poke bob 3");
    printf("Command result is zero if successful, is it? %d\n", ret);

    // Address not 4-byte aligned for poke
	ret = shell_evaluate("poke 11 0");
    printf("Command result is zero if successful, is it? %d\n", ret);

    // Set GPIO pin 47 (ACT LED) to output with FSEL register
    printf("FSEL register: pin 47 %d\n", gpio_read(GPIO_PIN47));
	ret = shell_evaluate("peek 0x20200010"); // check address before
	ret = shell_evaluate("poke 0x20200010 0x200000");  // fsel register
	ret = shell_evaluate("peek 0x20200010"); // check address after
    printf("Command result is zero if successful, is it? %d\n", ret);

    // Set GPIO pin 47 to high with SET 1 register
	ret = shell_evaluate("peek 0x20200038"); // Read LEV 1 register to check value before
	printf("After poke SET register pin 47: %d\n", gpio_read(GPIO_PIN47));
	ret = shell_evaluate("poke 0x20200020 0x8000"); // this sets gpio pin 47 to high 
    printf("Command result is zero if successful, is it? %d\n", ret);
	ret = shell_evaluate("peek 0x20200038"); // Read LEV 1 register to check value after

	timer_delay_ms(1000); // delay, so we can observe green light on

    // Set GPIO pin 47 to low with CLR 1 register
	printf("new pin 47 %d\n", gpio_read(GPIO_PIN47));
	ret = shell_evaluate("poke 0x2020002C 0x8000");
    printf("Command result is zero if successful, is it? %d\n", ret);
	ret = shell_evaluate("peek 0x20200038"); // Read LEV 1 reg to confirm back to original val
	printf("After poke CLR resister, pin 47: %d\n", gpio_read(GPIO_PIN47));

	timer_delay_ms(1000); // delay, so we can observe green light off

    // Test reboot command
    ret = shell_evaluate("reboot hello, world!");
    printf("Command result is zero if successful, is it? %d\n", ret);

}

// This is an example of a "fake" input. When asked to "read"
// next character, returns char from a fixed string, advances index
static unsigned char read_fixed(void)
{
    const char *input = "echo hello, wo\b\bworld\nhelp\n    spaces\n    \nthis input should be wayyyyyyyyyy too big, and so it really should call shell bell and give a proper warning!!!!\n";
    static int index;

    char next = input[index];
    index = (index + 1) % strlen(input);
    return next;
}

static void test_shell_readline_fixed_input(void)
{
    char buf[80];
    size_t bufsize = sizeof(buf);

    shell_init(read_fixed, printf); // input is fixed sequence of characters

    printf("\nTest shell_readline, feed chars from fixed string as input.\n");
    printf("readline> ");
    shell_readline(buf, bufsize);
    printf("readline> ");
    shell_readline(buf, bufsize);
    printf("readline> ");
    shell_readline(buf, bufsize);
    printf("readline> ");
    shell_readline(buf, bufsize);
    printf("readline> ");
    shell_readline(buf, bufsize);

}

static void test_shell_readline_keyboard(void)
{
    char buf[80];
    size_t bufsize = sizeof(buf);

    shell_init(keyboard_read_next, printf); // input from keybaord

    printf("\nTest shell_readline, type a line of input on ps2 keyboard.\n");
    printf("? ");
    shell_readline(buf, bufsize);


	/* Here, I tested inputs that were too big to ensure correct shell_bell. I also tried
	 * backspacing into nothing to ensure it called shell bell and did nothing. I tried hitting
	 * enter on an empty line which worked.
	 */
}

/* I used make run to test shell. Here, I tested typing in nothing, typing in only spaces,
 * adding leading whitespces/tabs and trailing whitespaces tabs before all commands. I tested
 * typing multiple arguments after a command and not enough commands. I tested typing in multiple
 * commands in sequence and also repeatedly using the same command.
 */

void main(void)
{
    uart_init();
    keyboard_init(KEYBOARD_CLOCK, KEYBOARD_DATA);

    printf("Testing keyboard and shell.\n");

    //test_keyboard_scancodes();
    //timer_delay_ms(500);

    //test_keyboard_sequences();
    //timer_delay_ms(500);

    test_keyboard_events();
    timer_delay_ms(500);

   // test_keyboard_chars();

    //test_keyboard_assert();

   // test_shell_evaluate();

    //test_shell_readline_fixed_input();

    //test_shell_readline_keyboard();

    printf("All done!\n");
    uart_putchar(EOT);
}
