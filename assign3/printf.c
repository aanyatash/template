/* Name: Aanya Tashfeen
 * Filename: printf.c
 * This file contains the code for a printf module.
 */
#include "printf.h"
#include <stdarg.h>
#include <stdint.h>
#include "strings.h"

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
	char temp[length + 1];
	memset(temp, '\0', sizeof(temp));
	int add_char = val;
	int i = 0;
	int digit = 0;
	if (add_char == 0) {
	    temp[0] = add_char + 48;
		i++;
	}
	while (add_char != 0) {
	    digit = add_char % base;
		if (digit < 10) {
		    temp[i] = digit + 48;
		}
		else {
		    temp[i] = digit + 87;
		}
		i++;
		add_char /= base;
		}
    temp[i] = '\0';
	int reverse = strlen(temp) - 1;
	i = 0;
    while (reverse >= 0) {
		str[i] = temp[reverse];
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
	unsigned int length = int_length(val);

    char str[length + 1];
	memset(str, '\0', sizeof(str));

    unsigned int padding = 0;

    num_to_base_str(str, val, base);

    // Leaves enough space for nullptr
	length = strlen(str);
	unsigned int max_length = min_width;
	if (length >  max_length) {
		max_length = length;
	}
	char max[max_length + 1];
    memset(max, '\0', sizeof(max));

    // pad with zeros
	if (length < min_width) {
		padding = min_width - length;
    }
    // adding number to end of max buffer
    for (int i = 0; i < length + 1; i++) {
		max[max_length - i] = str[length - i];
    } 
	if (padding > 0) {
		for (int i = 0; i < padding; i++) {
		    max[i] = 48;
		}
	}

	// move max to buf, think about truncation
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
	    char *buf_positive = buf + 1;
		buf[0] = '-';
		if (min_width == 0) {
		    min_width += 1;
		}
		if (bufsize == 1) {
		    buf[0] = '\0';
			bufsize += 1;
		}
		return 1 + unsigned_to_base(buf_positive, bufsize - 1, val*-1, base, min_width - 1);
	}
	return unsigned_to_base(buf, bufsize, val, base, min_width);
}

int vsnprintf(char *buf, size_t bufsize, const char *format, va_list args)
{
    
    return 0;
}

/* This function
 */
int snprintf(char *buf, size_t bufsize, const char *format, ...)
{
    int i = 0;
	int format_i = 0;

    char max[1024];
	size_t maxsize = sizeof(max);
	memset(max, '\0', maxsize);
	va_list ap;
	va_start(ap, format);
	while (format[format_i]) {
	    if (format[format_i] == '%') { // assumes any % sign always followed by valid code
		    if (format[format_i + 1] == 'c') {
				max[i] = va_arg(ap, int);
				max[i+1] = '\0'; // So strlen(max) can be used at end
			}
			else if (format[format_i + 1] == '%') {
				max[i] = '%';
				max [i+1] = '\0'; // So strlen(max) can be used at end
			}
			else if (format[format_i + 1] == 's') {
			    max[i] = '\0';
				char *str = va_arg(ap, char*);
				strlcat(max, str, maxsize);
			}
			else if (format[format_i + 1] == '0') {
			    const char* letter = NULL;
				unsigned int min_width = strtonum(format + format_i + 1, &letter);
				if (*letter == 'd') {
				    int val = va_arg(ap, int);
				    signed_to_base(max + i, maxsize - i, val, 10, min_width);
				    format_i += int_length(min_width) + 1;
				}
			    else if (*letter == 'x') {
				    int val = va_arg(ap, int);
				    signed_to_base(max + i, maxsize - i, val, 16, min_width);
				    format_i += int_length(min_width) + 1;
			    }
			}
			else if (format[format_i + 1] == 'd') {
				int val = va_arg(ap, int);
				signed_to_base(max + i, maxsize - i, val, 10, 0);
			}
			else if (format[format_i + 1] == 'x') {
				int val = va_arg(ap, int);
				signed_to_base(max + i, maxsize - i, val, 16, 0);
			}
			else if (format[format_i + 1] == 'p') {
				unsigned int val = va_arg(ap, unsigned int);
				max[i] = '0';
				max[i+1] = 'x';
				signed_to_base(max + i + 2, maxsize - i - 1, val, 16, 8);
			}
			i = strlen(max);
			format_i += 2;
		}
		else {
		    max[i] = format[format_i];
		    i++;
		    format_i++;
		}
	}
	va_end(ap);
	max[i] = '\0';
	buf[0] = '\0';

	return strlcat(buf, max, bufsize);
}

int printf(const char *format, ...)
{
    /* TODO: Your code here */
    return 0;
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
