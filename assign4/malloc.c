/* Name: Aanya Tashfeen
 * File: malloc.c
 * --------------
 * This is a dynamic memory allocator.
 * An allocation request is serviced by using sbrk to extend
 * the heap segment or by recycling memory.
 * It also coalesces free blocks when free is called, to ensure
 * that memory doesn't have small chunks of unused gaps.
 */

#include "malloc.h"
#include "printf.h"
#include <stddef.h> // for NULL
#include "strings.h"

extern int __bss_end__;

/*
 * The pool of memory available for the heap starts at the upper end of the
 * data section and extend up from there to the lower end of the stack.
 * It uses symbol __bss_end__ from memmap to locate data end
 * and calculates end of stack reservation assuming a 16MB stack.
 *
 * Global variables for the bump allocator:
 *
 * heap_start  location where heap segment starts
 * heap_end    location at end of in-use portion of heap segment
 *
 * count_allocs, count_frees, total_bytes_requested
 *          statistics tracked to assist with debug/validate heap
 */

// Initial heap segment starts at bss_end and is empty
static void *heap_start = &__bss_end__;
static void *heap_end = &__bss_end__;

// Private global variables to track heap statistics
static int count_allocs, count_frees, total_bytes_requested;

// Header for block of memory, total size = 8 bytes
struct header {
    size_t payload_size;
	int status; // 0 if free, 1 if in use
};

// Call sbrk as needed to extend size of heap segment
// Use sbrk implementation as given
void *sbrk(int nbytes)
{
    void *sp;
    __asm__("mov %0, sp" : "=r"(sp));   // get sp register (current stack top)
    char *stack_reserve = (char *)sp - 0x1000000; // allow for 16MB growth in stack

    void *prev_end = heap_end;
    if ((char *)prev_end + nbytes > stack_reserve) {
        return NULL;
    } else {
        heap_end = (char *)prev_end + nbytes;
        return prev_end;
    }
}


// Simple macro to round up x to multiple of n.
// The efficient but tricky bitwise approach it uses
// works only if n is a power of two -- why?
#define roundup(x,n) (((x)+((n)-1))&(~((n)-1)))

/* This function services a dynamic memory allocation request. The function
 * returns a pointer to to the address of memory block that has at least 'nbytes'
 * continguous bytes or NULL if the request cannot be satisfied. The block address is
 * aligned by an 8-byte boundary. If nbytes is 0, the function returns NULL.
 */
void *malloc (size_t nbytes)
{
    if (nbytes == 0) {
		return NULL;
	}
    count_allocs++;
    total_bytes_requested += nbytes;
    nbytes = roundup(nbytes, 8); // This ensures that heap-end is always 8-byte aligned

    // Move through heap from start to once it finds a freed block big enough for new alloc
	struct header *cur_header = (struct header *) heap_start;
	while (cur_header != heap_end) {
		if (cur_header->status == 0) { // only if block is free
		    // If either the empty block is exactly the same size
			// Or if the block is big enough to fit new alloc and another alloc of size 
			// at least 8 bytes with space for a new header (16 bytes total)
		    if (cur_header->payload_size == nbytes || cur_header->payload_size >= nbytes + 16) {
				cur_header->status = 1; // This block is now occupied
				size_t size = cur_header->payload_size;
				unsigned int remaining = size - nbytes; // How much remaining empty space in block after alloc
				cur_header->payload_size = nbytes;
				if (remaining != 0) { // if remainign room for new header and payload, add a new header
				    // Create a new header for remaining space above newly allocated block
				    struct header *new_header = cur_header + (size/8) + 1 - remaining/8;
				    new_header->payload_size = remaining - 8; // total remaining room minus 8-byte header
					new_header->status = 0; // This remaning room is unoccupied, so status is 0
				}
				return cur_header + 1; // Address to add payload, above header
			}
		}
		// Math to move to next header, as all headers form an implicit list
	    cur_header = cur_header + ((cur_header->payload_size)/8) + 1;
	}

	// If no large enough free block found, just add a new header and allocate the 
	// memory at the end of the heap
    cur_header = sbrk(nbytes + 8);
	// If cur_header is NULL, that means end of heap has been reached
	if (cur_header != NULL) {
		cur_header->payload_size = nbytes;
		cur_header->status = 1;
		return cur_header + 1;
	}
	return NULL;
}

/* This function deallocates the memory adddress 'ptr'. The function
 * will also coalesce any previosuly freed blocks above the newly freed block, 
 * to make one larger freed block.
 */
void free (void *ptr)
{
    count_frees++;
    if (ptr == NULL) {
		return;
	}
	struct header *header_free = ((struct header *) ptr) - 1;
	struct header *cur_header = header_free;
	int payload_to_add = 0;

	// Keep checking if next header is also free, until you either reach heap-end
	// or a header/block of memory that is occupied
    cur_header = cur_header + ((cur_header->payload_size)/8) + 1;
	while (cur_header != heap_end) {
		// If status == 1, so the a block that is occupied is reached, break
		// out of loop
		if (cur_header->status == 1) {
		    break;
		}
		// Math to work out size of new empty block in order to coalesce them
        payload_to_add += cur_header->payload_size + 8;
		// Math to move to next header, as all headers form an implicit list
        cur_header = cur_header + ((cur_header->payload_size)/8) + 1;
	}
	// New payload size, is the size of all the blocks to coalesce
	header_free->payload_size += payload_to_add;
	// Free the block by setting status to 0
	header_free->status = 0;
    
}

/* This function is a helper function that takes a string message as 
 * an input called 'label'. The heap dump function then prints this label
 * along with each header in the heap and a maximum of 16 bytes of content
 * stored with each header. This is a useful function for testing and debugging
 * as you ca visualize the contents of the heap.
 */
void heap_dump (const char *label)
{
    printf("\n---------- HEAP DUMP (%s) ----------\n", label);
    printf("Heap segment at %p - %p\n", heap_start, heap_end);
	printf("\n");
    struct header *cur_header = (struct header *) heap_start;
	char *cur_data;
	while (cur_header != heap_end) { // Loop through all the headers until the end
		printf("Payload size: %d\n", cur_header->payload_size);
		printf("Status: %d\n", cur_header->status);
        int i = 0;
		cur_data = (char *) (cur_header + 1);
		// Print data stored with cur_header. Prints a max of 16 bytes.
		while (i < 16 && i < cur_header->payload_size && cur_data != heap_end) {
		    printf("Current data: %c\n", cur_data[i]);
			i++;
		}
		printf("\n");
		if (cur_data == heap_end) {
		    printf("HEAP END");
		    break;
		}
		// Math to move to next header, as all headers form an implicit list
		cur_header = cur_header + ((cur_header->payload_size)/8) + 1;
	}
    printf("----------  END DUMP (%s) ----------\n", label);
    // Heap dump and stats should be consistent
    printf("Stats: %d in-use (%d allocs, %d frees), %d total bytes requested\n\n",
        count_allocs - count_frees, count_allocs, count_frees, total_bytes_requested);
}

void memory_report (void)
{
    printf("\n=============================================\n");
    printf(  "         Mini-Valgrind Memory Report         \n");
    printf(  "=============================================\n");
    // TODO: fill in for extension
}

void report_damaged_redzone (void *ptr)
{
    printf("\n=============================================\n");
    printf(  " **********  Mini-Valgrind Alert  ********** \n");
    printf(  "=============================================\n");
    printf("Attempt to free address %p that has damaged red zone(s):", ptr);
    // TODO: fill in for extension
}
