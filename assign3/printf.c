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

static unsigned int int_length (unsigned int val);
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

int unsigned_to_base(char *buf, size_t bufsize, unsigned int val, int base, size_t min_width)
{
    if (base != 10 && base != 16) {
		return 0;
	}
	unsigned int length = int_length(val);
	// Leaves enough space for nullptr
	unsigned int max_length = min_width;
	if (length >  max_length) {
		max_length = length;
	}
	char str[length + 1];
	char max[max_length + 1];
    memset(max, '\0', sizeof(max));
    unsigned int padding = 0;
	if (base == 10) {
		//convert integer to string
		str[length] = '\0';
		//int digit = 1;
		int add_char = val;
		for (int i = length - 1; i >=0; i--) {
			    str[i] = (add_char % 10) + 48;
				add_char /= 10;
		}
    }

	if (base == 16) {
		
	}

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
// buf + 1 and also min_width - 1 for minus sign, val*-1
    /* TODO: Your code here */
    return 0;
}

int vsnprintf(char *buf, size_t bufsize, const char *format, va_list args)
{
    /* TODO: Your code here */
    return 0;
}

int snprintf(char *buf, size_t bufsize, const char *format, ...)
{
    /* TODO: Your code here */
    return 0;
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
