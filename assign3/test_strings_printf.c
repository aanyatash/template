#include "assert.h"
#include "printf.h"
#include <stddef.h>
#include "strings.h"
#include "uart.h"

// Copied from printf.c
int unsigned_to_base(char *buf, size_t bufsize, unsigned int val, int base, size_t min_width);
int signed_to_base(char *buf, size_t bufsize, int val, int base, size_t min_width);

static void test_memset(void)
{
    char buf[12];
    size_t bufsize = sizeof(buf);

    memset(buf, 0x77, bufsize); // fill buffer with repeating value
    for (int i = 0; i < bufsize; i++)
        assert(buf[i] == 0x77); // confirm value
}

static void test_strcmp(void)
{
    assert(strcmp("apple", "apple") == 0);
    assert(strcmp("apple", "applesauce") < 0);
    assert(strcmp("pears", "apples") > 0);
	assert(strcmp("","") == 0);
	assert(strcmp("apples", "") > 0);
	assert(strcmp("", "pears") < 0);
	assert(strcmp("good", "goof") < 0);
}

static void test_strlcat(void)
{
    char buf[20];
    size_t bufsize = sizeof(buf);

    memset(buf, 0x77, bufsize); // init contents with known value

    buf[0] = '\0'; // null in first index makes empty string
    assert(strlen(buf) == 0);
    strlcat(buf, "CS", bufsize); // append CS
    assert(strlen(buf) == 2);
    assert(strcmp(buf, "CS") == 0);
    strlcat(buf, "107e", bufsize); // append 107e
    assert(strlen(buf) == 6);
    assert(strcmp(buf, "CS107e") == 0);
	
	// TEST 2 - size of final string exactly right
    char new[6];
	size_t newsize = sizeof(new);

	memset(new, 0x70, newsize);

	new[0] = 'h';
	new[1] = 'i';
	new[2] = '\0';
    assert(strlen(new) == 2);
	strlcat(new, "you", newsize);
	assert(strlen(new) == 5);
	assert(strcmp(new, "hiyou") == 0);


    // TEST 3 - concatenated final string
	char concat[6];
    size_t concatsize = sizeof(concat);

	memset(concat, 0x70, concatsize);

	concat[0] = 'h';
	concat[1] = 'i';
	concat[2] = '\0';
	assert(strlen(concat) == 2);
	strlcat(concat, "there", concatsize);
	assert(strlen(concat) == 5);
	assert(strcmp(concat, "hithe") == 0);
	assert(concat[5] == '\0');

    // TEST 4 - dst not proper string/dst size is wrong
	strlcat(concat, "", 3);
	assert(strlen(concat) == 5);
	assert(concat[5] == '\0');

    // TEST 5 - src is empty, dst size is correct
	strlcat(concat, "", concatsize);
	assert(strlen(concat) == 5);
	assert(concat[5] == '\0');

	// TEST 6 - no space in dst
	strlcat(concat, "m", concatsize);
	assert(strlen(concat) == 5);
	assert(concat[5] == '\0');

	// TEST 7 - add two null ptrs
//	char nullptr[3];
//	size_t nullptrsize = sizeof(nullptr);
//
//	memset(nullptr, '\0', nullptrsize);
//
//	assert(strlen(nullptr) == 0);
//	strlcat(nullptr, '\0', nullptrsize);
//	assert(strlen(nullptr) == 0);
//	assert(strcmp(nullptr, '\0') == 0);
}

static void test_strtonum(void)
{
    int val = strtonum("013", NULL);
    assert(val == 13);

    const char *input = "107rocks";
    const char *rest = NULL;

    val = strtonum(input, &rest);
    assert(val == 107);
    // rest was modified to point to first non-digit character
    assert(rest == &input[3]);

    // Invalid input 
    const char *wordptr = NULL;
	const char *word = "joker";
	val = strtonum(word, &wordptr);
	assert(val == 0);
	assert(word == &word[0]);


	// Empty string
    const char *emptyptr = NULL;
	const char *empty = "";
	val = strtonum(empty, &emptyptr);
	assert(val == 0);
	assert(emptyptr == &empty[0]);

	// No hex value
	const char *endptr = NULL;
	const char *no_hex = "0x";
	val = strtonum(no_hex, &endptr);
	assert(val == 0);
    assert(endptr == &no_hex[2]);
 
	// Invalid hex value
	const char *end = NULL;
	const char *invalid_hex = "0xi";
	val = strtonum(invalid_hex, &end);
	assert(val == 0);
	assert(end == &invalid_hex[2]);

	// Valid hex value
	const char *hex_end = NULL;
	const char *valid_hex = "0x7E";
	val = strtonum(valid_hex, &hex_end);
	assert(val == 126);
	assert(hex_end == &valid_hex[4]);
 
	// One letter invalid
	const char *one = NULL;
	const char *one_letter = "s";
	val = strtonum(one_letter, &one);
	assert(val == 0);
	assert(one == &one_letter[0]);

	// 0 in decimal
    const char *zero = NULL;
	const char *one_digit = "0";
	val = strtonum(one_digit, &zero);
	assert(val == 0);
	assert(zero == &one_digit[1]);


}

static void test_to_base(void)
{
    char buf[5];
    size_t bufsize = sizeof(buf);

    // Test base 10, normal
    memset(buf, 0x77, bufsize); // init contents with known value
    int n = unsigned_to_base(buf, bufsize, 35, 10, 0);
	assert(strcmp(buf, "35") == 0);
	assert(n == 2);

    // Test for base 10 but need padding
    buf[0] = '\0';
	n = unsigned_to_base(buf, bufsize, 35, 10, 3);
	assert(strcmp(buf, "035") == 0);
	assert(n == 3);

    // Test base 10, but truncation needed
	buf[0] = '\0';
	n = unsigned_to_base(buf, bufsize, 12345, 10, 3);
	assert(strcmp(buf, "1234") == 0);
	assert(n == 5);


    // Test base 16, normal
    buf[0] = '\0';
	n = unsigned_to_base(buf, bufsize, 126, 16, 0);
	assert(strcmp(buf, "7E") == 0);
	assert(n == 2);
	

	// Test base 16, need padding
	buf[0] = '\0';
	n = unsigned_to_base(buf, bufsize, 126, 16, 4);
	assert(strcmp(buf, "007E") == 0);
	assert(n == 4);

	// Test base 16, truncation and padding needed
	buf[0] = '\0';
	n = unsigned_to_base(buf, bufsize, 126, 16, 6);
	assert(strcmp(buf, "0000") == 0);
	assert(n == 6);

	// Test base 16, truncation needed
	char new[2];
	size_t  newsize = sizeof(new);
	n = unsigned_to_base(new, newsize, 126, 16, 0);
	assert(strcmp(new, "7") == 0);
	assert(n == 2);

	// Invalid base
	buf[0] = '\0';
    n = unsigned_to_base(buf, bufsize, 126, 8, 0);
	assert(n == 0);

    // buffer size of 0
    n = unsigned_to_base(buf, 0, 126, 10, 0);
	assert(n == 0);

    // DO WE WANT TO CHECK LENGTH OF BUFFER TO ENSURE CORRECT NULL TERMINATION
    // what if the number is more than a byte - why is max 1024???
    
    // buffer size of 1
	buf[0] = '\0';
	n = unsigned_to_base(buf, 1, 126, 10, 0);
	assert(n == 3);
	assert(strcmp(buf, "") == 0);

	buf[0] = '\0';
	n = signed_to_base(buf, bufsize, -9999, 10, 6);
    assert(strcmp(buf, "-099") == 0)
    assert(n == 6);
}

static void test_snprintf(void)
{
    char buf[100];
    size_t bufsize = sizeof(buf);

    memset(buf, 0x77, bufsize); // init contents with known value

    // Start off simple...
    snprintf(buf, bufsize, "Hello, world!");
    assert(strcmp(buf, "Hello, world!") == 0);

    // Decimal
    snprintf(buf, bufsize, "%d", 45);
    assert(strcmp(buf, "45") == 0);

    // Hexadecimal
    snprintf(buf, bufsize, "%04x", 0xef);
    assert(strcmp(buf, "00ef") == 0);

    // Pointer
    snprintf(buf, bufsize, "%p", (void *) 0x20200004);
    assert(strcmp(buf, "0x20200004") == 0);

    // Character
    snprintf(buf, bufsize, "%c", 'A');
    assert(strcmp(buf, "A") == 0);

    // String
    snprintf(buf, bufsize, "%s", "binky");
    assert(strcmp(buf, "binky") == 0);

    // Format string with intermixed codes
    snprintf(buf, bufsize, "CS%d%c!", 107, 'e');
    assert(strcmp(buf, "CS107e!") == 0);

    // Test return value
    assert(snprintf(buf, bufsize, "Hello") == 5);
    assert(snprintf(buf, 2, "Hello") == 5);
}

// This function just here as code to disassemble for extension
int sum(int n)
{
    int result = 6;
    for (int i = 0; i < n; i++) {
        result += i * 3;
    }
    return result + 729;
}

void test_disassemble(void)
{
    const unsigned int add = 0xe0843005;
    const unsigned int sub = 0xe24bd00c;
    const unsigned int mov = 0xe3a0006b;
    const unsigned int bne = 0x1afffffa;

    // If you have not implemented the extension, core printf
    // will output address not disassembled followed by I
    // e.g.  "... disassembles to 0x07ffffd4I"
    printf("Encoded instruction %x disassembles to %pI\n", add, &add);
    printf("Encoded instruction %x disassembles to %pI\n", sub, &sub);
    printf("Encoded instruction %x disassembles to %pI\n", mov, &mov);
    printf("Encoded instruction %x disassembles to %pI\n", bne, &bne);

    unsigned int *fn = (unsigned int *)sum; // diassemble instructions from sum function
    for (int i = 0; i < 10; i++) {
        printf("%p:  %x  %pI\n", &fn[i], fn[i], &fn[i]);
    }
}


void main(void)
{
    uart_init();
    uart_putstring("Start execute main() in tests/test_strings_printf.c\n");

    test_memset();
    test_strcmp();
    test_strlcat();
    test_strtonum();
    test_to_base();
    //test_snprintf();
    // test_disassemble();


    // TODO: Add more and better tests!

    uart_putstring("Successfully finished executing main() in tests/test_strings_printf.c\n");
    uart_putchar(EOT);
}
