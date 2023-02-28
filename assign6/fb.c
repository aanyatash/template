/* Name: Aanya Tashfeen
 * Filename: fb.c
 * This file contains the code for intializing a framebuffer
 * with single buffering and double buffering mode.
 */
#include "fb.h"
#include "assert.h"
#include "mailbox.h"

typedef struct {
    unsigned int width;       // width of the physical screen
    unsigned int height;      // height of the physical screen
    unsigned int virtual_width;  // width of the virtual framebuffer
    unsigned int virtual_height; // height of the virtual framebuffer
    unsigned int pitch;       // number of bytes per row
    unsigned int bit_depth;   // number of bits per pixel
    unsigned int x_offset;    // x of the upper left corner of the virtual fb
    unsigned int y_offset;    // y of the upper left corner of the virtual fb
    void *framebuffer;        // pointer to the start of the framebuffer
    unsigned int total_bytes; // total number of bytes in the framebuffer
} fb_config_t;

// fb is volatile because the GPU will write to it
static volatile fb_config_t fb __attribute__ ((aligned(16)));

/* This initializes the framebuffer. It takes 4 parameters:
 * 1) width is an unsigned int that describes the requested width in pixels of the
 * framebuffer.
 * 2) height is an unsigned int that describes the requested height in pixels of the
 * framebuffer.
 * 3) depth is an unsifned int that describes the requested depth in bytes of each pixel
 * 4) mode is an fb_mode_t enumerate which describes whther the framebuffer is single
 * buffered (FB_SINGLEBUFFER) or double buffered (FB_DOUBLEBUFFER) and the function
 * uses this to initialize the framebuffer accordingly.
 */
void fb_init(unsigned int width, unsigned int height, unsigned int depth_in_bytes, fb_mode_t mode)
{
    // initializing fb_config_t struct
    fb.width = width;
    fb.virtual_width = width;
    fb.height = height;
    fb.virtual_height = height;
    fb.bit_depth = depth_in_bytes * 8; // convert number of bytes to number of bits
    fb.x_offset = 0;
    fb.y_offset = 0;

    // the manual states that we must set these value to 0
    // the GPU will return new values in its response
    fb.pitch = 0;
    fb.framebuffer = 0;
    fb.total_bytes = 0;

	if (mode == 1) { // if in double-buffering mode
		fb.virtual_height *= 2;	
	}

    // Send address of fb struct to the GPU as message
    bool mailbox_success = mailbox_request(MAILBOX_FRAMEBUFFER, (unsigned int)&fb);
    assert(mailbox_success); // confirm successful config
}

/* 
 */
void fb_swap_buffer(void)
{
    if (fb.virtual_height == fb.height) {
		return;
	}
    if (fb.y_offset == 0) {
        fb.y_offset = fb.height;
	}
	else {
	    fb.y_offset = 0;
	}
    bool mailbox_success = mailbox_request(MAILBOX_FRAMEBUFFER, (unsigned int)&fb);
    assert(mailbox_success); // confirm successful config
}

/* This function returns the start address of framebuffer memory into which the client
 * can draw pixels. In single buffering mode, this is always the on-screen buffer. In
 * double-buffering mode, this is always the off-screen buffer.
 */
void* fb_get_draw_buffer(void)
{
    // If in double-buffering mode and the on-screen buffer is the top buffer
    if (fb.y_offset == 0 && fb.virtual_height != fb.height) {
	    // Pointer to beginning of off-screen buffer, which is below top buffer
        return ((unsigned char*) fb.framebuffer) + fb.pitch*fb.height;
	}
    return fb.framebuffer;
}

/* Return current width in pixels of framebuffer as an unsigned int.
 */
unsigned int fb_get_width(void)
{
    return fb.width;
}

/* Return current height in pixels of framebuffer as an unsigned int.
 */
unsigned int fb_get_height(void)
{
    return fb.height;
}

/* Return current depth in bytes of single pixel as an unsigned int.
 */
unsigned int fb_get_depth(void)
{
    return fb.bit_depth / 8; // Divide by 8 to convert from bits to bytes
}

/* Return the current pitch in bytes of a single row of pixels in
 * the framebuffer as an unsigned int. Pitch can be the width or greater
 * than it if the GPU adds padding to the end of the row.
 */
unsigned int fb_get_pitch(void)
{
    return fb.pitch;
}

