#include "backtrace.h"
#include "printf.h"

// consider adding struct of pc lr sp fp
// can do pointer arithmetic here

const char *name_of(uintptr_t fn_start_addr)
{
    // TODO: Fill in your code here.
    return "???";
}

// f is an array of frame_t's stored from top of stack to bottom
int backtrace (frame_t f[], int max_frames)
{
    frame_t current;
    // access frame pointer for inner most stack frame
	// this is equivalent to the saved pc address, so <func+ 12>
    uintptr_t *cur_fp;
    __asm__("mov %0, fp" : "=r" (cur_fp));
	printf("Start of frame of callee %p\n", cur_fp);

    int i = 0;
    while (*(cur_fp + 3) != 0) {
	    printf("Function #%d\n", i);
		// points at new frame pointer of caller in current stack frame
		int new_fp = *(cur_fp + 3); // dereferences to address of new pointer
		uintptr_t *caller_fp = (uintptr_t *)new_fp; // creates pointer at this address, the new fp
		printf("Start of frame of caller %p\n", caller_fp);

		// resume address points at lr of frame of cur frame
		uintptr_t caller_resume_addr = *(cur_fp + 1);
		printf("lr register of current frame, resume address of caller: 0x%x\n", caller_resume_addr);

		// resume offset = lr of current frame - pc of caller frame (frame above it on the stack) - 12
		int func_called = caller_resume_addr - new_fp - 12;
		printf("Lines into function %d\n", func_called);
		current.name = "??? ";
		current.resume_addr = caller_resume_addr;
		current.resume_offset = func_called;
		f[i] = current;
		cur_fp = caller_fp;
		printf("next fp %p\n", cur_fp);
		printf("\n");
		i++;
    }
	current.name = "end";
	current.resume_addr = 0;
	current.resume_offset = 0;
    f[i] = current;
    return i;
}

void print_frames (frame_t f[], int n)
{
    for (int i = 0; i < n; i++)
        printf("#%d 0x%x at %s+%d\n", i, f[i].resume_addr, f[i].name, f[i].resume_offset);
}

void print_backtrace (void)
{
    int max = 50;
    frame_t arr[max];

    int n = backtrace(arr, max);
    print_frames(arr+1, n-1);   // print frames starting at this function's caller
}
