/* Name: Aanya Tashfeen
 * Filename: keyboard.c
 * This file contains the code to read keyboard inputs and convert it 
 * to an event, taking into account active modifiers.
 */
#include "keyboard.h"
#include "ps2.h"

static ps2_device_t *dev;
static unsigned int modifier = 0;
static unsigned int press = 0;

/* This function initialized the keyboard and the clock and data gpio pins.
 */
void keyboard_init(unsigned int clock_gpio, unsigned int data_gpio)
{
    dev = ps2_new(clock_gpio, data_gpio);
}

/* This function uses the ps2_read function to return the scancode
 * based on the keys hit on the keyboard device.
 */
unsigned char keyboard_read_scancode(void)
{
    return ps2_read(dev);
}

/* This function provides a low level keyboard interface. It reads a 
 * sequence of scancode bytes, depending on whether a key was pressed
 * or released and which key it was. The function returns a 'key_action_t'
 * struct that represents the key action of the sequence read.
 */
key_action_t keyboard_read_sequence(void)
{
    key_action_t action = { 0 };
	unsigned char first_read = keyboard_read_scancode();

    // If f0 scancode, read next scancode
	// as this is a key release
	// The next scancode indicates what key was released
	if (first_read == 0xf0) {
		action.what = KEY_RELEASE;
		action.keycode = keyboard_read_scancode();
	}

	// If e0 scancode, read next scancode as this indicates an
	// extended key
	else if (first_read == 0xe0) {
	    unsigned char second_read = keyboard_read_scancode();
		// f0 indicates key release, so must read another scancode
		// to determine which key was released
	    if (second_read == 0xf0) {
		    action.what = KEY_RELEASE;
     		action.keycode = keyboard_read_scancode();

		}
		// If not f0, must be key press
		// The currently stored scancode, indicates the action
		else {
		    action.what = KEY_PRESS;
		    action.keycode = second_read;
		}
	}
	// Normal key pressed, just need to read one byte
    else {
		action.what = KEY_PRESS;
		action.keycode = first_read;
	}
	// if e0, read one after, key press - if e0, f0, read after, release
	// key press, scancode
    return action;
}

/* This function provides a mid level keybaord interface. The function reads
 * and blocks the next key event. It returns a key_event_t struct that represents
 * the key event. A key event is a rpess or release of a single key. The returned
 * struct also includes the ps2 key that was pressed or releases and the keyboard
 * modifiers in effect. Modifier keys don't generate events, they simply update a 
 * modifer variable that can track active modifiers.
 */
key_event_t keyboard_read_event(void)
{
    key_event_t event = { 0 };
	// Determines action
	key_action_t read = keyboard_read_sequence();

    // Keeps reading sequence of keys pressed until a non-modifier key is 
	// pressed, so it can generate an event
	while (read.keycode == 0x12 || read.keycode == 0x59 || read.keycode == 0x11 || 
				read.keycode == 0x14 || read.keycode == 0x58) {
		// If SHIFT key is pressed or released
		if (read.keycode == 0x12 || read.keycode == 0x59) {
		    // Add to active modfiers if pressed
		    if (read.what == KEY_PRESS) {
			    modifier = modifier | KEYBOARD_MOD_SHIFT;
		    }
			// Remover from active modifiers if released
		    else if (read.what == KEY_RELEASE) {
			    modifier = modifier & ~KEYBOARD_MOD_SHIFT;
		    }
		}

		// If ALT key is pressed or released
		else if (read.keycode == 0x11) {
		    // Add to active modifiers if alt is pressed
		    if (read.what == KEY_PRESS) {
			    modifier = modifier | KEYBOARD_MOD_ALT;
		    } 
			// Remove from active modifiers if released
		    else if (read.what == KEY_RELEASE) {
			    modifier = modifier & ~KEYBOARD_MOD_ALT;
		    }
		}

		// If CTRL key is pressed or released
		else if (read.keycode == 0x14) {
		    // Add to active modifiers if ctrl is pressed
		    if (read.what == KEY_PRESS) {
			    modifier = modifier | KEYBOARD_MOD_CTRL;
		    }
			// Remove from active modifiers
		    else if (read.what == KEY_RELEASE) {
			    modifier = modifier & ~KEYBOARD_MOD_CTRL;
		    }
		}

		// If CAPS LOCK is pressed
		// Only if it is pressed as this is a 'sticky' key

		// note if being pressed on and off
		else if (read.keycode == 0x58) {
		    // Check if caps lock is currently off by checking active modifiers
			// Only turn on if it isn't on - add to active modifiers
			if (read.what == KEY_RELEASE) {
				press = 0;
			}
		    if ((modifier & KEYBOARD_MOD_CAPS_LOCK) == 0 && read.what == KEY_PRESS && press == 0) {
		 	    modifier = modifier | KEYBOARD_MOD_CAPS_LOCK;
				press = 1;
		    }

			// Check if caps lock is on
			// If it is on, turn it off - remove from active modifiers
		    else if ((modifier & KEYBOARD_MOD_CAPS_LOCK) == KEYBOARD_MOD_CAPS_LOCK && read.what == KEY_PRESS && press == 0) {
			    modifier = modifier & ~KEYBOARD_MOD_CAPS_LOCK;
				press = 1;
		    }
		}
		// Read until a non-modifier key is pressed, so an event can be generated
		read = keyboard_read_sequence();

	}
    
	// Out of while loop, only returns event when it's a non-modifier key
	// Adds action, key, and modifiers to event struct
	event.action = read;
	event.key = ps2_keys[read.keycode];
	event.modifiers = modifier;
    return event;
}

/* This function provides a top level keyboard interface. The function reads and 
 * blocks the next typed key on the keyboard, while keeping track of active modifiers.
 * Return ASCII values in range 0 to 0x7f indicate a typed key with an ordinary ascii
 * character. A typed key that isn't associated with an ASCII character, returns a 
 * hex value over 0x90. The function calls keyboard_read_evemt to receive a key press
 * event.
 */
unsigned char keyboard_read_next(void)
{
    key_event_t read = keyboard_read_event();
	ps2_key_t character = read.key;
	key_action_t press = read.action;
	// Keep reading keyboard until a key is pressed
	// as no output should be written if release is registered
	while (press.what == KEY_RELEASE) {
		read = keyboard_read_event();
	    character = read.key;
		press = read.action;
	}
	// If shift is active, the alternate version of the typed character
	// should be outputted
	// Shift is checked first as shift is always prioritized
    if ((read.modifiers & KEYBOARD_MOD_SHIFT) == KEYBOARD_MOD_SHIFT) {
		return character.other_ch;
    }
	// If caps lock is active, the alternate version of the typed character
	// should be outputted only if it is an alphabetic letter. A capital letter
	// is outputted in this case.
    if ((read.modifiers & KEYBOARD_MOD_CAPS_LOCK) == KEYBOARD_MOD_CAPS_LOCK) {
	    // ASCII range for a through z
	    if (character.ch >= 97 && character.ch <= 122) {
		    return character.other_ch;
	    }
    }

	// If key is above 0x90 or no modifiers, this returns the ascii value
	// If key is just a regular character, it just returns the normal version
	// of the character as shift and caps lock are not active.
    return character.ch;

}
