#include "gpio.h"
#include "gpio_extra.h"
#include "keyboard.h"
#include "ps2.h"

static ps2_device_t *dev;

void keyboard_init(unsigned int clock_gpio, unsigned int data_gpio)
{
    // create new PS2 device
    dev = ps2_new(clock_gpio, data_gpio);
}

unsigned char keyboard_read_scancode(void)
{
    // read from PS2 device
    return ps2_read(dev);
}

