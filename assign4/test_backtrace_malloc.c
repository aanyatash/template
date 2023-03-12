#include "assert.h"
#include "backtrace.h"
#include "malloc.h"
#include "printf.h"
#include "rand.h"
#include <stdint.h>
#include "strings.h"
#include "uart.h"

void main(void);
void heap_dump(const char *label);
int mystery(void);

static void test_name_of(void)
{
    const char *name;

    name = name_of((uintptr_t)main);
    assert(strcmp(name, "main") == 0);
    name = name_of((uintptr_t)uart_init);
    assert(strcmp(name, "uart_init") == 0);
    name = name_of((uintptr_t)mystery); // function compiled without embedded name
    assert(strcmp(name, "???") == 0);
}

static void test_backtrace_simple(void)
{
    frame_t f[2];
    int frames_filled = backtrace(f, 2);

    assert(frames_filled == 2);
    assert(strcmp(f[0].name, "test_backtrace_simple") == 0);
    assert(f[0].resume_addr == (uintptr_t)test_backtrace_simple + f[0].resume_offset);
    assert(strcmp(f[1].name, "main") == 0);
    assert(f[1].resume_addr == (uintptr_t)main + f[1].resume_offset);
    printf("Here is a simple backtrace:\n");
    print_frames(f, frames_filled);
    printf("\n");
}


// Testing backtrace - main should look reasonable compared to other functions
// cstart is the same

static unsigned int factorial(int c) {
		if (c == 1) {
		    printf("\n");
		    print_backtrace(); // should have diff, factorial and recursion_backtrace frames
		    return 1;
		}
		return c * factorial(c-1);
}

static int diff(int a, int b) {
		return factorial (b - a);
}

static void test_more_recursion(void) {
		print_backtrace(); // should only print four frames as recursive functions have been popped
}

static void recursion_backtrace(void) {    
		int a = 1;
		int b = 5;
		diff(a, b);
	    printf("\n");
        test_more_recursion();
}

static int recursion_fun(int n)
{
    if (n == 0)
        return mystery();   // look in nameless.c
    else
        return 1 + recursion_fun(n-1);
}

static int test_backtrace_complex(int n)
{
    return recursion_fun(n);
}


// MALLOC TEST CASES SHOULD BE RUN INDIVIDUALLY TO BE ABLE TO TRACK
// INTENDED OUTPUT

static void test_heap_dump(void)
{
    heap_dump("Empty heap");

    int *p = malloc(sizeof(int));
    *p = 0;
    heap_dump("After p = malloc(4)");

    char *q = malloc(16);
    memcpy(q, "aaaaaaaaaaaaaaa", 16);
    heap_dump("After q = malloc(16)");

    free(p);
    heap_dump("After free(p)");

    free(q);
    heap_dump("After free(q)");
}

static void test_recyle(void) {

    // Tests how blocks are being recycled
	// Middle one is freed and newly allocated
	// blocks should take its space if they're small enough

    // allocate 16
    int *a = malloc(16);
	// allocate 32
	int *b = malloc(32);
	// allocate 16
    int *c = malloc(16);

	// loop through three times
	for (int i = 0; i < 3; i ++) {
	    // free middle
		free(b);
	    heap_dump("After free(b), which is malloc(32)");
	    // allocate 32
		int *d = malloc(32);
        heap_dump("After d = malloc(32), should take place of b space");
		b = d;
	}

	// free 32
	free(b);
	heap_dump("After free b, which is malloc(32)");
	// allocate 24 - should add to end
	int *e = malloc(24);
	heap_dump("After e = malloc(24), should add to end");
	// allocate 16 - should add to middle
	int *f = malloc(16);
	heap_dump("After f = malloc(16), should add to where b was, new header after e");
	// allocate 8 - should add to middle
	int *g = malloc(8);
	heap_dump("After g = malloc(8), should add to after f");

    free(g);
	free(f);
	free(e);
	free(c);
	free(a);
	heap_dump("Everything freed");

}

static void test_split(void) {

    // Testing to see how large free block is being split
	// to add smaller new blocks at beginning of stack
    int *large = malloc(83);
	heap_dump("After adding large, which is malloc(83)");
	free(large);
	heap_dump("After freeing large, which is malloc(83)");
    int *small_1 = malloc(8);
    heap_dump("After small, which is malloc(8)");
    int *small_2 = malloc(8);
    heap_dump("After small, which is malloc(8)");
    int *small_3 = malloc(8);
    heap_dump("After small, which is malloc(8)");
    int *small_4 = malloc(8);
    heap_dump("After small, which is malloc(8)");
    int *small_5 = malloc(8);
    heap_dump("After small, which is malloc(8)");
    int *small_6 = malloc(8);
    heap_dump("After small, which is malloc(8)");
    int *small_7 = malloc(8);
    heap_dump("After small, which is malloc(8)");
    int *small_8 = malloc(8);
    heap_dump("After small, which is malloc(8)");
    int *small_9 = malloc(8);
    heap_dump("After small, which is malloc(8)");
    int *small_10 = malloc(8);
    heap_dump("After small, which is malloc(8)");
	free(small_1);
	free(small_2);
	free(small_3);
	free(small_4);
	free(small_5);
	free(small_6);
	free(small_7);
	free(small_8);
	free(small_9);
    free(small_10);
}

static void test_coalesce(void) {

    // allocate small blocks
	// free in reverse order to monitor coalescing
	// request large block
    int *small_1 = malloc(4);
	int *small_2 = malloc(5);
	int *small_3 = malloc(5);
	int *small_4 = malloc(16);
	int *small_5 = malloc(8);
	free(small_5);
	heap_dump("After free small 5, which is malloc(8)");
	free(small_4);
	heap_dump("After free small 4, which is malloc(16)");
	free(small_3);
	heap_dump("After free small 3, which is malloc(5)");
	free(small_2);
	heap_dump("After free small 2, which is malloc(5)");
	free(small_1);
	heap_dump("After free small 1, which is malloc(4)");
	int *large_6 = malloc(16);
	heap_dump("After adding large 6, which is malloc(16)");
    free(large_6);
}

static void test_heap_simple(void)
{
    // allocate a string and array of ints
    // assign to values, check, then free
    const char *alphabet = "abcdefghijklmnopqrstuvwxyz";
    int len = strlen(alphabet);

    char *str = malloc(len + 1);
    memcpy(str, alphabet, len + 1);

    int n = 10;
    int *arr = malloc(n*sizeof(int));
    for (int i = 0; i < n; i++) {
        arr[i] = i;
    }

    assert(strcmp(str, alphabet) == 0);
    free(str);
    assert(arr[0] == 0 && arr[n-1] == n-1);
    free(arr);
}

static void test_heap_oddballs(void)
{
    // test oddball cases
    char *ptr;

    ptr = malloc(200000000); // request too large to fit
    assert(ptr == NULL); // should return NULL if cannot service request
    heap_dump("After reject too-large request");

    ptr = malloc(0); // legal request, though weird
	assert(ptr == NULL);
    heap_dump("After malloc(0)");
    free(ptr);

    free(NULL); // legal request, should do nothing
    heap_dump("After free(NULL)");
}

static void test_heap_multiple(void)
{
    // array of dynamically-allocated strings, each
    // string filled with repeated char, e.g. "A" , "BB" , "CCC"
    // Examine each string, verify expected contents intact.

    int n = 8;
    char *arr[n];

    for (int i = 0; i < n; i++) {
        int num_repeats = i + 1;
        char *ptr = malloc(num_repeats + 1);
        memset(ptr, 'A' - 1 + num_repeats, num_repeats);
        ptr[num_repeats] = '\0';
        arr[i] = ptr;
    }
    heap_dump("After all allocations");
    for (int i = n-1; i >= 0; i--) {
        int len = strlen(arr[i]);
        char first = arr[i][0], last = arr[i][len -1];
        assert(first == 'A' - 1 + len);  // verify payload contents
        assert(first == last);
        free(arr[i]);
    }
    heap_dump("After all frees");
}


void test_heap_redzones(void)
{
    // DO NOT ATTEMPT THIS TEST unless your heap has red zone protection!
    char *ptr;

    ptr = malloc(9);
    memset(ptr, 'a', 9); // write into payload
    free(ptr); // ptr is OK

    ptr = malloc(5);
    ptr[-1] = 0x45; // write before payload
    free(ptr);      // ptr is NOT ok

    ptr = malloc(12);
    ptr[13] = 0x45; // write after payload
    free(ptr);      // ptr is NOT ok
}


void main(void)
{
    //uart_init();
    //uart_putstring("Start execute main() in tests/test_backtrace_malloc.c\n");

    test_name_of();

    test_backtrace_simple();
    test_backtrace_simple(); // Again so you can see the main offset change!
    test_backtrace_complex(7);  // Slightly tricky backtrace
    recursion_backtrace(); // tricky backtrace

    test_heap_dump();
	//test_recyle();
	//test_split();

    //test_heap_simple();
	//test_coalesce();	
    //test_heap_oddballs();
    //test_heap_multiple();

    //test_heap_redzones(); // DO NOT USE unless you implemented red zone protection

    uart_putstring("\nSuccessfully finished executing main() in tests/test_backtrace_malloc.c\n");
    uart_putchar(EOT);
}
