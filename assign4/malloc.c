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
    // TODO: replace with your code
    count_allocs++;
    total_bytes_requested += nbytes;
    nbytes = roundup(nbytes, 8);
    return sbrk(nbytes);
}

void free (void *ptr)
{
    count_frees++;
    // TODO: fill in your own code here
}

void heap_dump (const char *label)
{
    printf("\n---------- HEAP DUMP (%s) ----------\n", label);
    printf("Heap segment at %p - %p\n", heap_start, heap_end);
    // TODO: fill in your own code here
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
