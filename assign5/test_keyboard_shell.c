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

    int ret = shell_evaluate("echo hello, world!");
    printf("Command result is zero if successful, is it? %d\n", ret);

	ret = shell_evaluate("help echo");
    printf("Command result is zero if successful, is it? %d\n", ret);

	ret = shell_evaluate("help");
    printf("Command result is zero if successful, is it? %d\n", ret);

	ret = shell_evaluate("help hi");
    printf("Command result is zero if successful, is it? %d\n", ret);

	ret = shell_evaluate("peek 0x8000");
    printf("Command result is zero if successful, is it? %d\n", ret);

	ret = shell_evaluate("peek");
    printf("Command result is zero if successful, is it? %d\n", ret);

	ret = shell_evaluate("peek bob");
    printf("Command result is zero if successful, is it? %d\n", ret);

	ret = shell_evaluate("peek 7");
    printf("Command result is zero if successful, is it? %d\n", ret);

	ret = shell_evaluate("poke 0x8000 1");
    printf("Command result is zero if successful, is it? %d\n", ret);

	ret = shell_evaluate("peek 0x8000");
    printf("Command result is zero if successful, is it? %d\n", ret);

	ret = shell_evaluate("poke 0x8000 0");
    printf("Command result is zero if successful, is it? %d\n", ret);

	ret = shell_evaluate("peek 0x8000");
    printf("Command result is zero if successful, is it? %d\n", ret);

	ret = shell_evaluate("poke 0x8000");
    printf("Command result is zero if successful, is it? %d\n", ret);

	ret = shell_evaluate("poke 0x8000 wilma");
    printf("Command result is zero if successful, is it? %d\n", ret);

	ret = shell_evaluate("poke bob 3");
    printf("Command result is zero if successful, is it? %d\n", ret);

	ret = shell_evaluate("poke 11 0");
    printf("Command result is zero if successful, is it? %d\n", ret);

    printf("pin 47 %d\n", gpio_read(GPIO_PIN47));
	ret = shell_evaluate("peek 0x20200010");

	ret = shell_evaluate("poke 0x20200010 0x200000");  // fsel register
    printf("Command result is zero if successful, is it? %d\n", ret);
	ret = shell_evaluate("peek 0x20200038");

	printf("new pin 47 %d\n", gpio_read(GPIO_PIN47));
	
	ret = shell_evaluate("poke 0x20200020 0x8000"); // this sets gpio pin 31 to high 
    printf("Command result is zero if successful, is it? %d\n", ret);
	ret = shell_evaluate("peek 0x20200038");


	printf("new pin 47 %d\n", gpio_read(GPIO_PIN47));


	ret = shell_evaluate("poke 0x2020002C 0x8000");
    printf("Command result is zero if successful, is it? %d\n", ret);
	ret = shell_evaluate("peek 0x20200038");


	printf("final pin 47 %d\n", gpio_read(GPIO_PIN47));


    ret = shell_evaluate("reboot hello, world!");
    printf("Command result is zero if successful, is it? %d\n", ret);

}

// This is an example of a "fake" input. When asked to "read"
// next character, returns char from a fixed string, advances index
static unsigned char read_fixed(void)
{
    const char *input = "echo hello, wo\b\bworld\nhelp\n";
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
}

static void test_shell_readline_keyboard(void)
{
    char buf[80];
    size_t bufsize = sizeof(buf);

    shell_init(keyboard_read_next, printf); // input from keybaord

    printf("\nTest shell_readline, type a line of input on ps2 keyboard.\n");
    printf("? ");
    shell_readline(buf, bufsize);
}

void main(void)
{
    uart_init();
    keyboard_init(KEYBOARD_CLOCK, KEYBOARD_DATA);

    printf("Testing keyboard and shell.\n");

    test_keyboard_scancodes();
    timer_delay_ms(500);

    test_keyboard_sequences();
    timer_delay_ms(500);

    test_keyboard_events(); // CHECK ESC
    timer_delay_ms(500);

    test_keyboard_chars();

    test_keyboard_assert();

    //test_shell_evaluate();

   // test_shell_readline_fixed_input();

    //test_shell_readline_keyboard();

    printf("All done!\n");
    uart_putchar(EOT);
}
