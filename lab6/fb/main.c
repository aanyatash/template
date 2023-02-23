#include "fb.h"
#include "uart.h"
#include "printf.h"

const int WIDTH = 640;
const int HEIGHT = 480;
const int DEPTH = 4;

void main(void)
{
    uart_init();
    printf("Initializing framebuffer.\n");
    fb_init(WIDTH, HEIGHT, DEPTH);
    printf("Framebuffer initialized. Exit.\n");
    uart_putchar(EOT);
}
