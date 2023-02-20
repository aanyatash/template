#include "keyboard.h"
#include "ps2.h"

static ps2_device_t *dev;

void keyboard_init(unsigned int clock_gpio, unsigned int data_gpio)
{
    dev = ps2_new(clock_gpio, data_gpio);
}

unsigned char keyboard_read_scancode(void)
{
    return ps2_read(dev);
}

key_action_t keyboard_read_sequence(void)
{
    key_action_t action = { 0 };
	unsigned char first_read = keyboard_read_scancode();
    // if f0, read second, key release, and store one after
	if (first_read == 0xf0) {
		action.what = KEY_RELEASE;
		action.keycode = keyboard_read_scancode();
	}
	else if (first_read == 0xe0) {
	    unsigned char second_read = keyboard_read_scancode();
	    if (second_read == 0xf0) {
		    action.what = KEY_RELEASE;
     		action.keycode = keyboard_read_scancode();

		}
		else {
		    action.what = KEY_PRESS;
		    action.keycode = second_read;
		}
	}
    else {
		action.what = KEY_PRESS;
		action.keycode = first_read;
	}
	// if e0, read one after, key press - if e0, f0, read after, release
	// key press, scancode
    return action;
}

key_event_t keyboard_read_event(void)
{
    // TODO: Your code here
    key_event_t event = { 0 };
    return event;
}

unsigned char keyboard_read_next(void)
{
    // TODO: Your code here
    return '!';
}
