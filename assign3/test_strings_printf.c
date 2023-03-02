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
	assert(strcmp("apples", "") == 97);
	assert(strcmp("", "pears") == -112);
	assert(strcmp("good", "goof") == -2);
}

static void test_strlcat(void)
{
    char buf[20];
    size_t bufsize = sizeof(buf);
	int total;

    memset(buf, 0x77, bufsize); // init contents with known value

    buf[0] = '\0'; // null in first index makes empty string
    assert(strlen(buf) == 0);
    total = strlcat(buf, "CS", bufsize); // append CS
    assert(strlen(buf) == 2);
	assert(total == 2);
    assert(strcmp(buf, "CS") == 0);
    total = strlcat(buf, "107e", bufsize); // append 107e
    assert(strlen(buf) == 6);
	assert(buf[6] == '\0');
	assert(total == 6);
    assert(strcmp(buf, "CS107e") == 0);

	// Test if buf size = 0 and nothing in buf
    total = strlcat(buf + 6, " work", 0);
	assert(total == 5);
	assert(strcmp(buf + 6, "") == 0);
	assert(strlen(buf+6) == 0);
	
	// size of final string exactly right to fit in buffer
    char new[6];
	size_t newsize = sizeof(new);

	memset(new, 0x70, newsize);

	new[0] = 'h';
	new[1] = 'i';
	new[2] = '\0';
    assert(strlen(new) == 2);
	total = strlcat(new, "you", newsize);
	assert(strlen(new) == 5);
	assert(total == 5);
	assert(strcmp(new, "hiyou") == 0);

    // concatenated final string
	char concat[6];
    size_t concatsize = sizeof(concat);

	memset(concat, 0x70, concatsize);

	concat[0] = 'h';
	concat[1] = 'i';
	concat[2] = '\0';
	assert(strlen(concat) == 2);
	total = strlcat(concat, "there", concatsize);
	assert(strlen(concat) == 5);
	assert(total == 7);
	assert(strcmp(concat, "hithe") == 0);
	assert(concat[5] == '\0');

    // dst size is wrong
	total = strlcat(concat, "", 3);
	assert(total == 3);
	assert(strlen(concat) == 5);
	assert(concat[5] == '\0');

    // src is empty, dst size is correct
	total = strlcat(concat, "", concatsize);
	assert(total == 5);
	assert(strlen(concat) == 5);
	assert(concat[5] == '\0');

	// no space in dst
	total = strlcat(concat, "m", concatsize);
	assert(total == 6);
	assert(strlen(concat) == 5);
	assert(concat[5] == '\0');

   // Test dstsize is 1
   char empty[2];
   empty[0] = '\0';
   total = strlcat(empty, "Hi", 1);
   assert(total == 2);
   assert(strlen(empty) == 0);
   assert(strcmp(empty, "") == 0);

   // Test dstsize is 0
   total = strlcat(empty, "Hello", 0);
   assert(total == 5);
   assert(strcmp(empty, "") == 0);

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
	assert(strlen(word) == 5);
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

    // Decimal number with 02
	const char *test_dec = NULL;
	const char *dec_number = "02";
    val = strtonum(dec_number, &test_dec);
	assert(val == 2);
	assert( test_dec == &dec_number[2] );
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
	n = unsigned_to_base(buf, bufsize, 35, 10, 3);
	assert(strcmp(buf, "035") == 0);
	assert(n == 3);

    // Test base 10, but truncation needed
	n = unsigned_to_base(buf, bufsize, 12345, 10, 3);
	assert(strcmp(buf, "1234") == 0);
	assert(n == 5);

	// Test base 10, min width equals size of number
	n = unsigned_to_base(buf, bufsize, 35, 10, 2);
	assert(strcmp(buf, "35") == 0);
	assert(n == 2);

    // Test base 16, normal
	n = unsigned_to_base(buf, bufsize, 126, 16, 0);
	assert(strcmp(buf, "7e") == 0);
	assert(n == 2);
	
    // Test base 16, min width equals size of number
	n = unsigned_to_base(buf, bufsize, 126, 16, 2);
	assert(strcmp(buf, "7e") == 0);
	assert(n == 2);

	// Test base 16, need padding
	n = unsigned_to_base(buf, bufsize, 126, 16, 4);
	assert(strcmp(buf, "007e") == 0);
	assert(n == 4);

	// Test base 16, truncation and padding needed
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
    n = unsigned_to_base(buf, bufsize, 126, 8, 0);
	assert(n == 0);

    // Buffer size of 0
    n = unsigned_to_base(buf, 0, 126, 10, 0);
	assert(n == 3);
 
    // Buffer size of 1
	n = unsigned_to_base(buf, 1, 126, 10, 0);
	assert(n == 3);
	assert(strcmp(buf, "") == 0);

	// Zero unsigned to base 10
	n = unsigned_to_base(buf, bufsize, 0, 10, 0);
    assert(n == 1);
	assert(strcmp(buf, "0") == 0);

	// Zero unsigned to base 16
	n = unsigned_to_base(buf, bufsize, 0, 16, 0);
	assert(n == 1);
	assert(strcmp(buf, "0") == 0);

    // Signed to base - min width specified, but bufsize is smaller
	n = signed_to_base(buf, bufsize, -9999, 10, 6);
    assert(strcmp(buf, "-099") == 0);
    assert(n == 6);

	// Signed to base, base 10, normal
    n = signed_to_base(buf, bufsize, -38, 10, 0);
	assert(strcmp(buf, "-38") == 0);
	assert(n == 3);

	// Signed, base 16, normal
    n = signed_to_base(buf, bufsize, -126, 16, 0);
	assert(strcmp(buf, "-7e") == 0);
	assert(n == 3);

    // 0 signed
    n = signed_to_base(buf, bufsize, 0, 16, 0);
	assert(strcmp(buf, "0") == 0);
	assert(n == 1);

	// 1 but calling signed function
    n = signed_to_base(buf, bufsize, 1, 10, 0);
	assert(strcmp(buf, "1") == 0);
    assert(n == 1);

	// Zero signed but min width 3
	n = signed_to_base(buf, bufsize, 0, 10, 3);
	assert(strcmp(buf, "000") == 0);
    assert(n == 3);

	// -10 signed, base 16, min width 2 (hex value a)
	n = signed_to_base(buf, bufsize, -10, 16, 2);
	assert(strcmp(buf, "-a") == 0);
	assert(n == 2);

	// -15 signed, base 16, min width 2 (hex value f)
	n = signed_to_base(buf, bufsize, -15, 16, 2);
	assert(strcmp(buf, "-f") == 0);
	assert(n == 2);

	// -1 signed, min width 1, boundary case
    n = signed_to_base(buf, bufsize, -1, 10, 1);
	assert(strcmp(buf, "-1") == 0);
	assert(n == 2);

	// Signed, but buffer too small
    n = signed_to_base(new, newsize, -1, 10, 0);
	assert(strcmp(new, "-") == 0);
	assert(n == 2);

	// Signed, but buffer too small and need padding
	n = signed_to_base(new, newsize, -1, 10, 3);
	assert(strcmp(new, "-") == 0);
	assert(n == 3);
    
	// Signed, but bufsize = 2, so when calls unsigned function, bufsize is 1
    n = signed_to_base(buf, 2, -1, 10, 0);
	assert(strcmp(buf, "-") == 0);
	assert(n == 2);

	// Signed, bufsize = 0
	buf[0] = '\0';
	n = signed_to_base(buf, 0, -1, 10, 0);
	assert(strcmp(buf, "") == 0); // buf should be same as before/unchanged
	assert(n == 2);

	// Signed, bufsize = 1 (as bufsize would be 0 for unsigned)
    n = signed_to_base(buf, 1, -1, 10, 0);
	assert(strcmp(buf, "") == 0);
	assert(n == 2);
}

static void test_snprintf(void)
{
    char buf[100];
    size_t bufsize = sizeof(buf);
	int total;

    memset(buf, 0x77, bufsize); // init contents with known value

    // Start off simple...
    total = snprintf(buf, bufsize, "Hello, world!");
    assert(strcmp(buf, "Hello, world!") == 0);
	assert(total == 13);

    // Decimal
    total = snprintf(buf, bufsize, "%d", 45);
    assert(strcmp(buf, "45") == 0);
	assert(total == 2);
    
	memset(buf, 0x97, bufsize);
	// Decimal with min width
	total = snprintf(buf, bufsize, "%05d", 30);
	assert(strcmp(buf, "00030") == 0);
	assert(total == 5);

    // Hexadecimal with min width
    total = snprintf(buf, bufsize, "%04x", 0xef);
    assert(strcmp(buf, "00ef") == 0);
	assert(total = 4);

	// Hexadecimal without min width
	total = snprintf(buf, bufsize, "%x", 126);
	assert(strcmp(buf, "7e") == 0);
	assert(total == 2);

	// Min width equals same as number of digits in decimal
	total = snprintf(buf, bufsize, "%03d", 126);
    assert(strcmp(buf, "126") == 0);
	assert(total == 3);

	// Test negative digits for decimal
	total = snprintf(buf, bufsize, "%d", -38);
	assert(strcmp(buf, "-38") == 0);
	assert(total == 3);

	// Test negative number with specified min width - dec
	total = snprintf(buf, bufsize, "%04d", -7);
	assert(strcmp(buf, "-007") == 0);
	assert(total == 4);

	// Test char, string, and digit with specified width
    total = snprintf(buf, bufsize, "%c picked up %03d %s.",'I', 5, "eggs");
	assert(strcmp(buf, "I picked up 005 eggs.") == 0);
	assert(total == strlen(buf));

    // Pointer
    total = snprintf(buf, bufsize, "%p", (void *) 0x20200004);
    assert(strcmp(buf, "0x20200004") == 0);
	assert(total == 10);

    // Character
    total = snprintf(buf, bufsize, "%c", 'A');
    assert(strcmp(buf, "A") == 0);
	assert(total == 1);

    // String
    total = snprintf(buf, bufsize, "%s", "binky");
    assert(strcmp(buf, "binky") == 0);
	assert(total == 5);

    // Format string with intermixed codes
    total = snprintf(buf, bufsize, "CS%d%c!", 107, 'e');
    assert(strcmp(buf, "CS107e!") == 0);
	assert(total == 7);

    // Test return value
    assert(snprintf(buf, bufsize, "Hello") == 5);
    assert(snprintf(buf, 2, "Hello") == 5);

	// Character and percent
	total = snprintf(buf, bufsize, "%c%c = 100%% fresh", 'C', 'S');
    assert(strcmp(buf, "CS = 100% fresh") == 0);
	assert(total == 15);
    
    // Buffer size too small
    char new[5];
	size_t newsize = sizeof(new);
	total = snprintf(new, newsize, "Hi, %s!", "Aanya");
	assert(strcmp(new, "Hi, ") == 0);
	assert(total == 10);

    // Test large string
    total = snprintf(buf, bufsize, "The %s is %05d%c away from %p", "lazy fox", 100, 'm', (void *) 0x2000020);
	assert(strcmp(buf, "The lazy fox is 00100m away from 0x02000020") == 0);
	assert(total == strlen(buf));
	assert(buf[strlen(buf)] == '\0');

	// bufsize = 1
    total = snprintf(buf, 1, "I'm in y%de%d", 2, 2);
	assert(strcmp(buf, "") == 0);
	assert(total == 11);

	// buf size = 0
	buf[0] = '\0';
	total = snprintf(buf, 0, "Hi, %s", "Aanya");
	assert(strcmp(buf, "") == 0);
	assert(total = 9);

    // big hex value 
	total = snprintf(buf, 10, "%x", 0x9abcdef0);
	assert(strcmp(buf, "9abcdef0") == 0);

    // big hex value
	total = snprintf(buf, 20,"%x", 0xffffffff);
	assert(strcmp(buf, "ffffffff") == 0);

    // bufsize = 0
	char dst[0];
	char comp[0];
    total = snprintf(dst, 0, "No room at the inn");
	assert(strcmp(dst, comp) == 0);
    
	// Pointer
	char point[20];
    total = snprintf(point, 20, "%p", (void *)0x7f94);
    assert(strcmp(point, "0x00007f94") == 0);

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
    test_snprintf();
    test_disassemble();


    uart_putstring("Successfully finished executing main() in tests/test_strings_printf.c\n");
    uart_putchar(EOT);
}
