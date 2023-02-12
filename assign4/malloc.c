/*
 * File: malloc.c
 * --------------
 * This is the simple "bump" allocator from lecture.
 * An allocation request is serviced by using sbrk to extend
 * the heap segment.
 * It does not recycle memory (free is a no-op) so when all the
 * space set aside for the heap is consumed, it will not be able
 * to service any further requests.
 *
 * This code is given here just to show the very simplest of
 * approaches to dynamic allocation. You are to replace this code
 * with your own heap allocator implementation.
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

void *malloc (size_t nbytes)
{
    count_allocs++;
    total_bytes_requested += nbytes;
    nbytes = roundup(nbytes, 8);
	struct header add_header;


	// iterate through heap from heap_start
	// look at headers
	// find header with status 0 or end of heap
	// if status 0, check if payload size equal to n bytes or greater than nbytes + 16 (enough space for new header and payload)
	// change status to 1
	// change payload size
	// if space remaning is not 0, add new header with status 0 and payload_size at end of block

	struct header *cur_header = (struct header *) heap_start;
	while (cur_header != heap_end) {
		if (cur_header->status == 0) {
		    if (cur_header->payload_size == nbytes || cur_header->payload_size >= nbytes + 16) {
				cur_header->status = 1;
				size_t size = cur_header->payload_size;
				unsigned int remaining = size - nbytes;
				cur_header->payload_size = nbytes;
				if (remaining != 0) {
				    struct header *new_header = cur_header + (size/8) + 1 - remaining/8;
				    new_header->payload_size = remaining - 8;
					new_header->status = 0;
				}
				return cur_header + 1;
			}
		}
	    cur_header = cur_header + ((cur_header->payload_size)/8) + 1;
	}

    cur_header = sbrk(nbytes + 8);
	cur_header->payload_size = nbytes;
	cur_header->status = 1;
	return cur_header + 1;
}

void free (void *ptr)
{
    count_frees++;
    if (ptr == NULL) {
		return;
	}
	struct header *header_free = ((struct header *) ptr) - 1;
	header_free->status = 0;
    
}

void heap_dump (const char *label)
{
    printf("\n---------- HEAP DUMP (%s) ----------\n", label);
    printf("Heap segment at %p - %p\n", heap_start, heap_end);
	printf("\n");
    struct header *cur_header = (struct header *) heap_start;
	char *cur_data;
	// read header
	// printf payload which is digit
	// printf status which is also digit
	// printf data, if payload > 16, only print 16 -- use while loop to subtract from size
	while (cur_header != heap_end) {
		printf("Payload size: %d\n", cur_header->payload_size);
		printf("Status: %d\n", cur_header->status);
        int i = 0;
		cur_data = (char *) (cur_header + 1);
		while (i < 16 && i < cur_header->payload_size) {
		    printf("Current data: %c\n", cur_data[i]);
			i++;
		}
		printf("\n");
		cur_header = cur_header + ((cur_header->payload_size)/8) + 1;
	}
    printf("----------  END DUMP (%s) ----------\n", label);
    // heap dump and stats should be consistent
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
