/* Name: Aanya Tashfeen
 * Filename: backtrace.c
 * This file contains the code for a backtrace module.
 * Backtrace prints out the sequence of currently active
 * function calls, along with information of the resume address
 * and how many instructions down the funciton (in bytes) it called
 * the next function.
 */

#include "backtrace.h"
#include "printf.h"

/* This function finds the name of a function and
 * returns it as a pointer to the statring address
 * of the name. If no name is stored, it returns
 * "???". The function takes the starting address
 * of one instruction before the start of of the 
 * the function (<function+0>) as a parameter.
 */
const char *name_of(uintptr_t fn_start_addr)
{
    // Pointer to the start address of where the name is stored
    uintptr_t* name = (uintptr_t *) fn_start_addr;
    if (((*name >> 24) & 0xff) == 0xff) { // Check is MSB is 0xFF, as this signifies name storage
		int length = (*name & ~0xff000000); // Find length of name of function
		name = name - (length/4); // Pointer arithmetic to find starting address of function_name
        // Type-cast to char*, so name can be stored as chars from ascii
		const char* function_name = (const char*) name;
		return function_name;
	}
    return "???"; // Nameless function as no name stored before function prologue
}

/* This function writes to 'f' which is an array of frame_t's that 
 * store information on functions in the stack from top of stack to
 * bottom of the stack. 'f' contains information on the return addresses
 * of each function and how far below in the function it called another function.
 * 'f' also contains information on the name of the function. It will write either
 * 'max_frames' number of frames onto the array, or the total number of frames, depending
 * on which number comes first. The function returns an integer of the number of frames
 * it wrote to the array 'f'.
 */
int backtrace (frame_t f[], int max_frames)
{
    frame_t current;

    uintptr_t *cur_fp;
    __asm__("mov %0, fp" : "=r" (cur_fp)); // cur_fp for backtrace function, top of the stack
	uintptr_t new_fp = *(cur_fp - 3); // Find top of stack frame for function that called backtrace, address int val
	uintptr_t callee_lr = *(cur_fp - 1); // Return address for caller from backtrace link register
	uintptr_t *caller_fp = (uintptr_t *)new_fp; // Pointer to top of frame for function that called backtrace
	cur_fp = caller_fp;

	uintptr_t name_addr;
	int func_called;

    int i = 0;
    while (*(cur_fp - 3) != 0 && i != max_frames) { // Iterate until top of stack reached (stop at cstart frame)

        // The address of the name of the function is stored 16 bytes before the stored pc value in
		// the stack frame
	    name_addr = (*cur_fp) - 16;
	
		// Points at frame pointer of caller of function in current stack frame
		new_fp = *(cur_fp - 3); // dereferences to address of new pointer
		caller_fp = (uintptr_t *)new_fp; // creates pointer at this address

		// Finds lr of current stack frame - this is resume address for the caller of this function
		uintptr_t caller_lr = *(cur_fp - 1);

		// resume offset = lr in callee frame (frame below cur frame, as this is resume address of cur function)
		// minus the pc of current frame minus 12 (as pc is always <func+12>
		// This is the number of bytes down another function was called in the function
		func_called = callee_lr - ((*cur_fp) - 12); 
 
		// Adds frame to 'f' array
		current.name = name_of(name_addr);
		current.resume_addr = callee_lr;
		current.resume_offset = func_called;
		f[i] = current;

		// Sets up loop for next stack frame
		cur_fp = caller_fp;
		callee_lr = caller_lr;
		i++;
    }

    if (i != max_frames) {
		// For _cstart frame
		name_addr = (*cur_fp) - 16;
		func_called = callee_lr - ((*cur_fp) - 12);
		current.name = name_of(name_addr);
		current.resume_addr = callee_lr;
		current.resume_offset = func_called;
		f[i] = current;
		i++;
	}
    return i;
}

/* This function prints an integer 'n' number of frames stored in the
 * array 'f'.
 */
void print_frames (frame_t f[], int n)
{
    for (int i = 0; i < n; i++)
        printf("#%d 0x%x at %s+%d\n", i, f[i].resume_addr, f[i].name, f[i].resume_offset);
}

/* This function prints a maximum of 50 frames after performing a backtrace, beginning
 * at this function's caller.
 */
void print_backtrace (void)
{
    int max = 50;
    frame_t arr[max];

    int n = backtrace(arr, max);
    print_frames(arr+1, n-1);   // print frames starting at this function's caller
}
