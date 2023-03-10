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

void decode_instruction(char* buf, size_t bufsize, unsigned int *addr);
static int rotate(int immediate, int rot); 

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
	unsigned int add_char = val;
	int i = 0;
	unsigned int digit = 0;

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
    if (bufsize == 0) {
		return strlen(max);
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
	if (val < 0) {

		// Accounts for min_width = 0
		// to avoid passing a negative value into unsigned to base
		if (min_width == 0) {
		    min_width += 1;
		}


		if (bufsize == 0) {
 		    return 1 + unsigned_to_base(buf, bufsize, val*-1, base, min_width - 1);
		}

		char *buf_positive = buf + 1; // Leaves space for minus sign
		buf[0] = '-';

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
				    unsigned_to_base(max + i, maxsize - i, val, 16, min_width);
				    format_i += int_length(min_width) + 1;
			    }
			}
			else if (format[format_i + 1] == 'd') { // decimal digit formatting code
				int val = va_arg(args, int);
				signed_to_base(max + i, maxsize - i, val, 10, 0);
			}
			else if (format[format_i + 1] == 'x') { // hexadecimal digit formatting code
				unsigned int val = va_arg(args, unsigned int);
				unsigned_to_base(max + i, maxsize - i, val, 16, 0);   // problem: it converts it to a signed integer
			}
			else if (format[format_i + 1] == 'p') { // pointer address formatting code
				unsigned int val = va_arg(args, unsigned int);
		        if (format[format_i + 2] == 'I') {
				    format_i += 1;
					decode_instruction(max + i, maxsize - i,(unsigned int *) val);
				}
				else {
				    // appends address after '0x'
				    max[i] = '0';
				    max[i+1] = 'x';
				    unsigned_to_base(max + i + 2, maxsize - i - 2, val, 16, 8); 
				}
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
	if (bufsize != 0) {
	    buf[0] = '\0'; // strlcat only works on strings with nullptr
	}

	int total = strlcat(buf, max, bufsize);
	return total;
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
	return total;
}


/* The struct insn bitfield is declared using exact same layout as bits are organized in
 * the encoded instruction. Accessing struct.field will extract just the bits
 * apportioned to that field. If you look at the assembly the compiler generates
 * to access a bitfield, you will see it simply masks/shifts for you. Neat!
*/

static const char *cond[16] = {"eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc",
                               "hi", "ls", "ge", "lt", "gt", "le", "", ""};

static const char *opcodes[16] = {"and", "eor", "sub", "rsb", "add", "adc", "sbc", "rsc",
                                  "tst", "teq", "cmp", "cmn", "orr", "mov", "bic", "mvn"};

static const char *reg[16] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10",
						      "fp", "ip", "sp", "lr", "pc"};								

static const char *sh[4] = {"lsl", "lsr", "asr", "ror"}; // shift operations

static const char *branch[2] = {"b", "bl"}; // branch or branch link?

static const char *mem[2] = {"str", "ldr"}; // load or store?

static const char *w[2] = {"", "!"}; // for write-back operation

static const char *u[2] = {"-", ""}; // for adding or subtracting offset

static const char *b[2] = {"", "b"}; // for byte transfer operation

// bit masking for data processing instruction
struct insn  {
    uint32_t reg_op2:4; // Rm is R3 for additional register
    uint32_t one:1; // default 0
    uint32_t shift_op: 2;  // operation shift - two bits could be LSL, LSR, ASR, ROR
    uint32_t shift: 5;  // shift immediate value - 5 bit shift operation
    uint32_t reg_dst:4; // destination register
    uint32_t reg_op1:4; // source register
    uint32_t s:1;      // is the flag set? i.e. is condition met? 
    uint32_t opcode:4; // instruction to execute
    uint32_t imm:1;  // Is there an immediate value?
    uint32_t kind:2; // data processing or branch or load/store?
    uint32_t cond:4; // conditional suffix
};

// bit masking for branch instruction
struct branch_insn  {
    int32_t offset:24; // offset for finding address of return instruction
	uint32_t link:1; // is it link or not? 1 for link
	uint32_t one:1; // part of kind for branch - should be one
    uint32_t kind:2; // data processing or load/store?
    uint32_t cond:4; // conditional suffix
};

// bit masking for bx instruction
struct bx_insn {
    uint32_t reg_dst:4; // register to find return address
    uint32_t middle:24; // bits to identify bx instruction
	uint32_t cond:4; // conditional suffix
};

// bit masking for load/store memory instruction
struct mem_insn  {
    uint32_t shift_imm: 12;  // shift immediate value
	uint32_t reg_src:4; // source register
    uint32_t reg_dst:4; // destination register
    uint32_t L:1; // if 1 ldr, else str
    uint32_t W:1; // write back bit (if 1 write address into base)
    uint32_t B:1; // byte/word bit (if 1 transfer byte quantity, else word quantity)
    uint32_t U:1; // up/down bit (if 1, add offset to base. else, subtract.)
    uint32_t P:1; // pre/post indexing bit
    uint32_t imm:1;  // Is there an immediate value?
    uint32_t kind:2; // data processing, branch, or load/store?
    uint32_t cond:4; // conditional suffix
};

struct shift {
    uint32_t reg_op2:4; // Rm is offset register
    uint32_t one:1; // default 0
    uint32_t shift_op: 2;  // operation shift - two bits could be LSL, LSR, ASR, ROR
    uint32_t shift: 5;  // shift immediate value - 5 bit shift operation
};

/* This function performs a 32-bit right rotation, given a immediate
 * value to a rotate and rotate value which must be multiplied by 2
 * as per the rules for decoding a shift value.
 */
int rotate(int immediate, int rot) {
    rot = (rot*2) % 32;
    int shift = immediate >> rot;
    int rotate = immediate << (32 - rot);
    return shift | rotate;
}


/* This function takes the address of an instruction, dereferences it, and then
 * decodes it and stores it in buf.
 */
void decode_instruction(char* buf, size_t bufsize, unsigned int *addr) { 
    // addr is address of instruction
	if (bufsize != 0) {
	    buf[0] = '\0';
	}
	if (bufsize == 0 || bufsize == 1) {
		return;
	}
    struct insn in = *(struct insn *)addr;  // derefernce address to get code

    // Branch instruction - not exchange
	if (in.kind == 0b10 && in.imm == 1) {
	    struct branch_insn branch_in = *(struct branch_insn *)addr; // use branch struct for bit masking
		// Calculate offset using address of instruction as pc address anchor
		// imm24 << 2 + pc + 8
		snprintf(buf, bufsize, "%s%s 0x%x", branch[branch_in.link], cond[branch_in.cond], (branch_in.offset << 2) + ((unsigned int) addr + 8));
	}

    // Data processing instructions
	else if (in.kind == 0) {
	    // this check for a bx lr instruction first - teq is actually equivalent to this!
	    if (in.imm == 0 && ((in.opcode & 0b1000) == 0b1000)) {
		     struct bx_insn bx_in = *(struct bx_insn *)addr; // bx struct for bit masking
			 if (bx_in.middle == 0b000100101111111111110001) { // bit pattern for bx instruction
				snprintf(buf, bufsize, "bx%s %s", cond[bx_in.cond], reg[bx_in.reg_dst]);
				return;
			 }
		}
		// These instructions only need two register values after the instruction name
		if (strcmp(opcodes[in.opcode], "mov") == 0 || strcmp(opcodes[in.opcode], "mvn") == 0 
				|| strcmp(opcodes[in.opcode], "cmp") == 0 || strcmp(opcodes[in.opcode], "cmn") == 0 
				|| strcmp(opcodes[in.opcode], "tst") == 0 || strcmp(opcodes[in.opcode], "teq") == 0)  {
				unsigned int reg1 = in.reg_op1;
				if (strcmp(opcodes[in.opcode], "mov") == 0 || strcmp(opcodes[in.opcode], "mvn") == 0 ) {
				    reg1 = in.reg_dst; // mov and mvn use the register destination as parameters (exception from others)
				}    
				// no immediate value and no shifting on register value
				// two registers
				if (in.imm == 0 && in.shift == 0 && in.one == 0) {
				    snprintf(buf, bufsize, "%s%s %s, %s", opcodes[in.opcode], cond[in.cond], reg[reg1], reg[in.reg_op2]);
				}

				// no immediate value, shifting operation required on a number
				// two registers plus a shifted immediate value
				else if (in.imm == 0 && in.shift != 0 && in.one == 0) {
				    snprintf(buf, bufsize, "%s%s %s, %s, %s #%d", opcodes[in.opcode], cond[in.cond], reg[reg1], 
					reg[in.reg_op2], sh[in.shift_op], in.shift);
				}

				// no immediate value but there is shifting on a register value
				// two registers plus a shifted register
				else if (in.imm == 0 && in.one == 1) {
				    snprintf(buf, bufsize, "%s%s %s, %s, %s %s", opcodes[in.opcode], cond[in.cond], reg[reg1], 
					reg[in.reg_op2], sh[in.shift_op], reg[in.shift >> 1]);
				}

				// There is an immediate value, need to perform RRX to get correct immediate value
				else if (in.imm == 1) {
				    // the bit mask we use doesn't completely apply to this instr
					// do some bit shifting to correct this
				    unsigned int rot = in.shift >> 1; // rot is 4 bits
					// immediate is remaining 8 bits
				    unsigned int immediate = (in.shift & 0b0001) << 7 | in.shift_op << 5 | in.one << 4 | in.reg_op2;
				    unsigned int val = immediate;
				    if (rot != 0) {
						val = rotate(immediate, rot); // perform rotation operation
				    }
						snprintf(buf, bufsize, "%s%s %s, #%d", opcodes[in.opcode], cond[in.cond], reg[reg1], val);
				}
		}
    
	    // no immediate value and no shifting on register value
		// three registers
		else if (in.imm == 0 && in.shift == 0 && in.one == 0) {
		    snprintf(buf, bufsize, "%s%s %s, %s, %s", opcodes[in.opcode], cond[in.cond], reg[in.reg_dst], 
			reg[in.reg_op1], reg[in.reg_op2]);
		}

		// no immediate value, shifting operation required on number
		// three registers plus a shifted immediate value
		else if (in.imm == 0 && in.shift != 0 && in.one == 0) {
		    snprintf(buf, bufsize, "%s%s %s, %s, %s, %s #%d", opcodes[in.opcode], cond[in.cond], 
			reg[in.reg_dst], reg[in.reg_op1], reg[in.reg_op2], sh[in.shift_op], in.shift);
		}

		// no immediate value but there is shifting on a register value
		// three registers plus a shifted register
		else if (in.imm == 0 && in.one == 1) {
		    snprintf(buf, bufsize, "%s%s %s, %s, %s, %s %s", opcodes[in.opcode], cond[in.cond], 
			reg[in.reg_dst], reg[in.reg_op1], reg[in.reg_op2], sh[in.shift_op], reg[in.shift >> 1]);
		}

        // There is an immediate value, need to perform RRX to get correct immediate value
		else if (in.imm == 1) {
		    // the bit mask we use doesn't completely apply to this instruction
			// do some but shifting to correct this
		    unsigned int rot = in.shift >> 1; // rot is 4 bits
			// immediate is remaining 8 bits
			int immediate = (in.shift & 0b0001) << 7 | in.shift_op << 5 | in.one << 4 | in.reg_op2;
			int val = immediate;
			if (rot != 0) {
			    val = rotate(immediate, rot);
			}
		    snprintf(buf, bufsize, "%s%s %s, %s, #%d", opcodes[in.opcode], cond[in.cond], reg[in.reg_dst], 
			reg[in.reg_op1], val);
		}

	}
	// ldr, str, ldrb, and strb instructions
	else if (in.kind == 0b01) {
	    struct mem_insn mem_in = *(struct mem_insn *)addr; // use memory instructions struct to bit mask
		if (mem_in.P == 1) { // pre-indexing instr - so add offset before transfer
		    // No immediate value and no shifting involved
		    if (mem_in.imm == 0 && mem_in.shift_imm == 0) {
		        snprintf(buf, bufsize, "%s%s%s %s, [%s]%s", mem[mem_in.L], cond[mem_in.cond], 
				b[mem_in.B], reg[mem_in.reg_src], reg[mem_in.reg_dst], w[mem_in.W]);
		    }
			// no register shifting, but there is an immediate offset to add
			else if (mem_in.imm == 0 && mem_in.shift_imm != 0) {
		        snprintf(buf, bufsize, "%s%s%s %s, [%s, #%s%d]%s", mem[mem_in.L], cond[mem_in.cond],  
				b[mem_in.B], reg[mem_in.reg_src], reg[mem_in.reg_dst], u[mem_in.U], mem_in.shift_imm, w[mem_in.W]);
		    }
			else { // there is an immediate value in this case, so a register shift to add
			    // must use different mapping struct for an immeidate value shift
				// this is because the bit masking is different for the last two bits
			    unsigned int store_shift = mem_in.shift_imm;
			    unsigned int *shift = &store_shift;
		        struct shift in_shift = *(struct shift *)shift;
				// no shift to be applied, just an additional register
		        if (mem_in.imm == 1 && in_shift.shift == 0) {
		            snprintf(buf, bufsize, "%s%s%s %s, [%s, %s%s]%s", mem[mem_in.L], cond[mem_in.cond], b[mem_in.B], 
				    reg[mem_in.reg_src], reg[mem_in.reg_dst], u[mem_in.U], reg[in_shift.reg_op2], w[mem_in.W]);
		        }
				// shift operation to be applied to additional register param
		        else if (mem_in.imm == 1 && mem_in.shift_imm != 0) {
		            snprintf(buf, bufsize, "%s%s%s %s, [%s, %s, %s #%s%d]%s", mem[mem_in.L], cond[mem_in.cond], 
				    b[mem_in.B], reg[mem_in.reg_src], reg[mem_in.reg_dst], reg[in_shift.reg_op2], sh[in_shift.shift_op], u[mem_in.U], 
				    in_shift.shift, w[mem_in.W]);
		    }  
			}
		}
		else if (mem_in.P == 0) { // post-indexing instruction - so add offset after transfer
		    // No immediate value and no shifting involved
		    if (mem_in.imm == 0 && mem_in.shift_imm == 0) {
		        snprintf(buf, bufsize, "%s%s%s %s, [%s]", mem[mem_in.L], cond[mem_in.cond], b[mem_in.B], 
				reg[mem_in.reg_src], reg[mem_in.reg_dst]);
		    }
		    else if (mem_in.imm == 0 && mem_in.shift_imm != 0) {
		        snprintf(buf, bufsize, "%s%s%s %s, [%s], #%s%d", mem[mem_in.L], cond[mem_in.cond],  b[mem_in.B], 
				reg[mem_in.reg_src], reg[mem_in.reg_dst], u[mem_in.U], mem_in.shift_imm);
		    }
			else { // there is an immediate value in this case, so a register shift to add
			    // must use different mapping struct for an immeidate value shift
				// this is because the bit masking is different for the last two bits
			    unsigned int store_shift = mem_in.shift_imm;
			    unsigned int *shift = &store_shift;
		        struct shift in_shift = *(struct shift *)shift;
				// no shift to be applied, just an additional register
		        if (mem_in.imm == 1 && in_shift.shift == 0) {
		            snprintf(buf, bufsize, "%s%s%s %s, [%s], %s%s", mem[mem_in.L], cond[mem_in.cond], b[mem_in.B], 
				    reg[mem_in.reg_src], reg[mem_in.reg_dst], u[mem_in.U], reg[in_shift.reg_op2]);
		        }
		        // shift operation to be applied to additional register param
		        else if (mem_in.imm == 1 && mem_in.shift_imm != 0) {
		            snprintf(buf, bufsize, "%s%s%s %s, [%s], %s %s #%s%d", mem[mem_in.L], cond[mem_in.cond], b[mem_in.B], 
				    reg[mem_in.reg_src], reg[mem_in.reg_dst], reg[in_shift.reg_op2], sh[in_shift.shift_op], u[mem_in.U], mem_in.shift_imm);
		        }
			}
		}
	}
	else {
		snprintf(buf, bufsize, "%x", *addr);
	}
}
