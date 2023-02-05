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


// Find length helper function
unsigned int int_length (unsigned int val) {

    unsigned int length = 0;
	if (val == 0) {
		length = 1;
	}
	unsigned int tmp = val;
	while (tmp) {
		length++;
		tmp = tmp / 10;
	}
	return length;

}

void num_to_base_str(char *str, unsigned int val, int base) {

    unsigned int length = int_length(val);
	char temp[length + 1];
	memset(temp, '\0', sizeof(temp));
	int add_char = val;
	int i = 0;
	int digit = 0;
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
    strlcat(buf, max, bufsize);
	if (bufsize < max_length + 1) {
	    return max_length;
	}
    return strlen(buf);
}

int signed_to_base(char *buf, size_t bufsize, int val, int base, size_t min_width)
{
    if (bufsize == 0) {
		return 0;
	}
	if (val < 0) {
	    char *buf_positive = buf + 1;
		buf[0] = '-';
		return 1 + unsigned_to_base(buf_positive, bufsize - 1, val*-1, base, min_width - 1);
	}
	return unsigned_to_base(buf, bufsize, val, base, min_width);
}

int vsnprintf(char *buf, size_t bufsize, const char *format, va_list args)
{
    /* TODO: Your code here */
    return 0;
}

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
	    if (format[format_i] == '%') {
		// include error checking for i + 1
		    if (format[format_i + 1] == 'c') {
				max[i] = va_arg(ap, int);
				format_i += 2;
				i += 1;
			}
			else if (format[format_i + 1] == '%') {
				max[i] = '%';
				format_i += 2;
				i += 1;
			}
			else if (format[format_i + 1] == 's') {
			    max[i] = '\0';
				char *str = va_arg(ap, char*);
				strlcat(max, str, maxsize);
				i = strlen(max);
				format_i += 2;
			}
			else if (format[format_i + 1] == '0') {
			    const char* letter = NULL;
				unsigned int min_width = strtonum(format + format_i + 1, &letter);
				if (*letter == 'd') {
					max[i] = '\0';
				    int val = va_arg(ap, int);
				    signed_to_base(max + i, maxsize - i, val, 10, min_width);
				    i = strlen(max);
				    format_i += int_length(min_width) + 3;
				}
			    else if (*letter == 'x') {
				    max[i] = '\0';
				    int val = va_arg(ap, int);
				    signed_to_base(max + i, maxsize - i, val, 16, min_width);
				    i = strlen(max);
				    format_i += int_length(min_width) + 3;
			    }
			}
			else if (format[format_i + 1] == 'd') {
				max[i] = '\0';
				int val = va_arg(ap, int);
				signed_to_base(max + i, maxsize - i, val, 10, 0);
				i = strlen(max);
				format_i += 2;
			}
			else if (format[format_i + 1] == 'x') {
				max[i] = '\0';
				int val = va_arg(ap, int);
				signed_to_base(max + i, maxsize - i, val, 16, 0);
				i = strlen(max);
				format_i += 2;
			}
			else if (format[format_i + 1] == 'p') {
				int val = va_arg(ap, int);
				max[i] = '0';
				max[i+1] = 'x';
				signed_to_base(max + i + 2, maxsize - i - 1, val, 16, 0);
				i = strlen(max);
				format_i += 2;
			}
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
	strlcat(buf, max, bufsize);
    return strlen(buf);
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
