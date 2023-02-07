/* Name: Aanya Tashfeen
 * Filename: printf.c
 * This file contains the code for a printf module. The main functions include
 * signed_to_base, vsn_printf, and printf.
 */
#include "printf.h"
#include <stdarg.h>
#include <stdint.h>
#include "strings.h"
#include "uart.h"

/** Prototypes for internal helpers.
 * Typically these would be qualified as static (private to module)
 * but, in order to call them from the test program, we declare them externally
 */
int unsigned_to_base(char *buf, 
                     size_t bufsize, 
                     unsigned int val, 
                     int base, size_t 
                     min_width);
int signed_to_base(char *buf, 
                   size_t bufsize, 
                   int val, 
                   int base, 
                   size_t min_width);

static unsigned int int_length(unsigned int val);
static void num_to_base_str(char *str, unsigned int val, int base);

#define MAX_OUTPUT_LEN 1024


/* This helper function for unsigned_to_base finds the  number of digits in a 
 * number (length) and returns this number as an unsigned
 * integer. The function has one parameter which is an
 * unisgned integer called val which is the number we are
 * trying to determine the length of.
 */
unsigned int int_length (unsigned int val) {

    unsigned int length = 0;

	// Accounts for 0 case
	if (val == 0) {
		return 1;
	}

	unsigned int tmp = val;
	// Keeps dividing by 10 until the answer is 0
	// Which gives the number of digits in a number
	while (tmp) {
		length++;
		tmp = tmp / 10;
	}
	return length;

}

/* This helper function for unsigned_to_base converts an integer into a string of numbers
 * in a specified base. In this case, the base should be either 10
 * or 16. The function has three parameters: a char* which is a pointer
 * to a string of characters that will store the result, and unsigned
 * integer which is the value to be converted, and an integer which is the
 * base to convert to.
 */
void num_to_base_str(char *str, unsigned int val, int base) {

    unsigned int length = int_length(val);
	char temp[length + 1]; // temporary array to store reverse conversion
	memset(temp, '\0', sizeof(temp)); // Initialize memory to nullptrs
	int add_char = val;
	int i = 0;
	int digit = 0;

	// Account for val equals 0 case
	if (add_char == 0) {
	    temp[0] = add_char + 48; // ASCII value for 0
		i++;
	}

	while (add_char != 0) {
	    digit = add_char % base; // Find remainder
		if (digit < 10) {
		    temp[i] = digit + 48; // 48 is ASCII value for 0
		}
		else {
		    temp[i] = digit + 87; // 87 is ASCII value for a - 10
		}
		i++;
		add_char /= base; // Move onto next digit
		}
    temp[i] = '\0';
	int reverse = strlen(temp) - 1; // Index to reverse string
	i = 0;
    while (reverse >= 0) {
		str[i] = temp[reverse]; // str is final string format
		reverse--;
		i++;
	}
	str[i] = '\0';

}

/* The function unsigned_to_base converts an unsigned integer to a string of numbers in a
 * specified base with a specified minimum width, adding zeros to ensure this is reached. 
 * The function returns an integer which is the count of characters added to the
 * string or the count of characters that should've been added to the string if the buf 
 * size wasn't too small. If the bufsize is too small, the string of characters is truncated.
 * The function takes 5 parameters: a char* array of pointers to store the string of numbers, the
 * size of this array, an unsigned integer value to be conevrted, the integer base of conversion, 
 * and the minimum size the final string of numbers must be.
 */
int unsigned_to_base(char *buf, size_t bufsize, unsigned int val, int base, size_t min_width)
{
    if (base != 10 && base != 16) {
		return 0;
	}
	if (bufsize == 0) {
		return 0;
	}
	unsigned int length = int_length(val); // Number of digits in number

    char str[length + 1];
	memset(str, '\0', sizeof(str)); // Initialize memory to nullptrs

    unsigned int padding = 0;

    num_to_base_str(str, val, base); // Stores base conversion as string in str

    // Leaves enough space for nullptr
	length = strlen(str);
	unsigned int max_length = min_width;
	if (length >  max_length) {
		max_length = length;
	}

	// Creates a space in memory that is the max size of the string
	char max[max_length + 1];
    memset(max, '\0', sizeof(max));

    // Calculate no of zeros to pad with, if length less than min width
	if (length < min_width) {
		padding = min_width - length;
    }
    // Adds number to end of max buffer
    for (int i = 0; i < length + 1; i++) {
		max[max_length - i] = str[length - i];
    } 
	// Pad with zeros if necessary to reach min width
	if (padding > 0) {
		for (int i = 0; i < padding; i++) {
		    max[i] = 48;
		}
	}

	// Move max to buf, strlcat truncates if necessary
	buf[0] = '\0';
    return strlcat(buf, max, bufsize);
}


/* This function is the exact same as unsigned to base, except it converts
 * a signed integer to a string of numbers in a specific base. If the number is
 * negative, the first character in the string is always a minus sign. This function
 * similarly returns an integer which is the count of characters added to the
 * string or the count of characters that should've been added to the string if the buf 
 * size wasn't too small.
 */
int signed_to_base(char *buf, size_t bufsize, int val, int base, size_t min_width)
{
    if (bufsize == 0) {
		return 0;
	}
	if (val < 0) {
	    char *buf_positive = buf + 1; // Leaves space for minus sign
		buf[0] = '-';

		// Accounts for min_width = 0
		// to avoid passing a negative value into unsigned to base
		if (min_width == 0) {
		    min_width += 1;
		}

		// Accounts for bufsize = 1 to maintain correct return value from unsigned_to_base
		// as this prevents a bufsize of 0 being passed in
		if (bufsize == 1) {
		    buf[0] = '\0';
			bufsize += 1;
		}
		return 1 + unsigned_to_base(buf_positive, bufsize - 1, val*-1, base, min_width - 1);
	}
	return unsigned_to_base(buf, bufsize, val, base, min_width);
}

/* This function constructs a formatted output string to 'buf' using an input
 * string and a va_list of arguments. It writes the output string to the 
 * destination buffer. It takes 4 parameters: the char pointer array 'buf' which
 * specifies the memory location, the size of this array in a size_t 'bufsize', the 
 * format string which is a const char pointer array, and a va_list of args. The function
 * returns the number of characters expected to have been written to 'buf'.
 */
int vsnprintf(char *buf, size_t bufsize, const char *format, va_list args)
{
    int i = 0;
	int format_i = 0;

    char max[1024]; // Create max size buffer
	size_t maxsize = sizeof(max);
	memset(max, '\0', maxsize); // Initialize memory to nullptrs

	while (format[format_i]) {
	    if (format[format_i] == '%') { // assumes any % sign always followed by valid code
		    if (format[format_i + 1] == 'c') {
				max[i] = va_arg(args, int);
				max[i+1] = '\0'; // So strlen(max) can be used at end
			}
			else if (format[format_i + 1] == '%') {
				max[i] = '%';
				max [i+1] = '\0'; // So strlen(max) can be used at end
			}
			else if (format[format_i + 1] == 's') { // String formatting code
			    max[i] = '\0'; // strlcat only works if nullptr present
				char *str = va_arg(args, char*);
				strlcat(max, str, maxsize);
			}
			else if (format[format_i + 1] == '0') { // Digit formatting with specified width
			    const char* letter = NULL;

				// Finds number (which is min_width) by using strtonum to search
				// string starting at '0'. Letter points to finishing location on string.
				unsigned int min_width = strtonum(format + format_i + 1, &letter);
				if (*letter == 'd') { // decimal digit formatting code
				    int val = va_arg(args, int);
				    signed_to_base(max + i, maxsize - i, val, 10, min_width);
				    format_i += int_length(min_width) + 1;
				}
			    else if (*letter == 'x') { // hexadecimal digit formatting code
				    int val = va_arg(args, int);
				    signed_to_base(max + i, maxsize - i, val, 16, min_width);
				    format_i += int_length(min_width) + 1;
			    }
			}
			else if (format[format_i + 1] == 'd') { // decimal digit formatting code
				int val = va_arg(args, int);
				signed_to_base(max + i, maxsize - i, val, 10, 0);
			}
			else if (format[format_i + 1] == 'x') { // hexadecimal digit formatting code
				int val = va_arg(args, int);
				signed_to_base(max + i, maxsize - i, val, 16, 0);
			}
			else if (format[format_i + 1] == 'p') { // pointer address formatting code
				unsigned int val = va_arg(args, unsigned int);
				// appends address after '0x'
				max[i] = '0';
				max[i+1] = 'x';
				signed_to_base(max + i + 2, maxsize - i - 1, val, 16, 8); 
				}
			i = strlen(max); // Only adds to end of max
			format_i += 2; // Skips %f on format string, where f is formatting code
		}
		else {
		    max[i] = format[format_i];
		    i++;
		    format_i++;
		}
	}

	max[i] = '\0';
	buf[0] = '\0'; // strlcat only works on strings with nullptr

	return strlcat(buf, max, bufsize);
}

/* This function takes a variable number of parameters. It takes in a char
 * pointer array 'buf', a size_t which desribes the size of this array, a char
 * pointer array 'format' string, and then a variable number of arguments. The
 * function constructs a list of these variable arguments to pass into vsnprintf,
 * which is not a variadic function. This function returns an integer that describes
 * the number of characters that is expected to be written into 'buf', regardless of
 * bufsize.
 */
int snprintf(char *buf, size_t bufsize, const char *format, ...)
{
    va_list args;
	va_start(args, format); // indicates that list of arguments starts after the format param
	int result = vsnprintf(buf, bufsize, format, args);
	va_end(args);
	return result;
}

/* This function takes in a format string and a variable number of arguments. It returns
 * an integer, which is the number of characters uart is expected to write to the terminal.
 * This function calls vsnprintf.
 */
int printf(const char *format, ...)
{
    // Allocates max memory
    char buf[1024];
	size_t bufsize = sizeof(buf);
	memset(buf, '\0',  bufsize); // Initialize memory to nullptrs
    va_list args;
	va_start(args, format); // indicates that list of args starts after format param
	int total = vsnprintf(buf, bufsize, format, args);
	va_end(args);
	uart_putstring(buf); // Writes to terminal
	//memset(buf, '\0', bufsize);
	return total;
}


/* From here to end of file is some sample code and suggested approach
 * for those of you doing the disassemble extension. Otherwise, ignore!
 *
 * The struct insn bitfield is declared using exact same layout as bits are organized in
 * the encoded instruction. Accessing struct.field will extract just the bits
 * apportioned to that field. If you look at the assembly the compiler generates
 * to access a bitfield, you will see it simply masks/shifts for you. Neat!


static const char *cond[16] = {"eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc",
                               "hi", "ls", "ge", "lt", "gt", "le", "", ""};
static const char *opcodes[16] = {"and", "eor", "sub", "rsb", "add", "adc", "sbc", "rsc",
                                  "tst", "teq", "cmp", "cmn", "orr", "mov", "bic", "mvn"};

struct insn  {
    uint32_t reg_op2:4;
    uint32_t one:1;
    uint32_t shift_op: 2;
    uint32_t shift: 5;
    uint32_t reg_dst:4;
    uint32_t reg_op1:4;
    uint32_t s:1;
    uint32_t opcode:4;
    uint32_t imm:1;
    uint32_t kind:2;
    uint32_t cond:4;
};

static void sample_use(unsigned int *addr) {
    struct insn in = *(struct insn *)addr;
    printf("opcode is %s, s is %d, reg_dst is r%d\n", opcodes[in.opcode], in.s, in.reg_dst);
}

*/
