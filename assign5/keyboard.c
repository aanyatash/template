#include "keyboard.h"
#include "ps2.h"

static ps2_device_t *dev;
static unsigned int modifier = 0;

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
    key_event_t event = { 0 };
	key_action_t read = keyboard_read_sequence();
	// action
	// key - ch and other_ch - ps2_keys[hex]
	// modifiers
    // keep reading sequence until not one of the modifier keys
	// shift
	while (read.keycode == 0x12 || read.keycode == 0x59 || read.keycode == 0x11 || 
				read.keycode == 0x14 || read.keycode == 0x58) {
		// add shift to active modifiers
		if (read.keycode == 0x12 || read.keycode == 0x59) {
		    if (read.what == KEY_PRESS) {
			    modifier = modifier | KEYBOARD_MOD_SHIFT;
		    }
		    else if (read.what == KEY_RELEASE) {
			    modifier = modifier & ~KEYBOARD_MOD_SHIFT;
		    }
		}

		// alt - 0x11
		else if (read.keycode == 0x11) {
		    if (read.what == KEY_PRESS) {
			    modifier = modifier | KEYBOARD_MOD_ALT;
		    } 
		    else if (read.what == KEY_RELEASE) {
			    modifier = modifier & ~KEYBOARD_MOD_ALT;
		    }
		}

		// ctrl
		else if (read.keycode == 0x14) {
		    if (read.what == KEY_PRESS) {
			    modifier = modifier | KEYBOARD_MOD_CTRL;
		    }
		    else if (read.what == KEY_RELEASE) {
			    modifier = modifier & ~KEYBOARD_MOD_CTRL;
		    }
		}

		// caps lock
		else if (read.keycode == 0x58 && read.what == KEY_PRESS) {
		    // caps lock on check right shift 3 and then & with 1
		    if ((modifier & KEYBOARD_MOD_CAPS_LOCK) == 0) {
		 	    modifier = modifier | KEYBOARD_MOD_CAPS_LOCK;
		    }
		    else if ((modifier & KEYBOARD_MOD_CAPS_LOCK) == KEYBOARD_MOD_CAPS_LOCK) {
			    modifier = modifier & ~KEYBOARD_MOD_CAPS_LOCK;
		    }
		}
		read = keyboard_read_sequence();

	}
    
	//out of while loop, only returns event when it's a non-modifier key
	event.action = read;
	event.key = ps2_keys[read.keycode];
	event.modifiers = modifier;
    return event;
}

unsigned char keyboard_read_next(void)
{
    key_event_t read = keyboard_read_event();
	ps2_key_t character = read.key;
	key_action_t press = read.action;
	while (press.what == KEY_RELEASE) {
		read = keyboard_read_event();
	    character = read.key;
		press = read.action;
	}
    if ((read.modifiers & KEYBOARD_MOD_SHIFT) == KEYBOARD_MOD_SHIFT) {
		return character.other_ch;
    }
    if ((read.modifiers & KEYBOARD_MOD_CAPS_LOCK) == KEYBOARD_MOD_CAPS_LOCK) {
	    if (character.ch >= 97 || character.ch <= 122) {
		    return character.other_ch;
	    }
    }

	// if key is above 0x90 or no modifiers, return the ascii value
	// if shift pressed, return keys[1]
	// if caps pressed and keys[0] between ascii values 0 through z
    return character.ch;

}
