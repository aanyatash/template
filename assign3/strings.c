/* Name: Aanya Tashfeen
 * Filename: strings.c
 * This file contains the code for a strings library. The functions in this
 * file can create a copy of a string, set an array to an arbitrary repeating
 * value, find the length of a string, compare to strings, and concatenate two
 * strings together.
 */

#include "strings.h"

/* This function copies contents from src to dst one byte
 * at a time. It takes three parameters: dst is a pointer array
 * to be filled, src is a pointer array to be copied, and n is an
 * unsigned size_t integer which specifies the number of bytes to
 * be copied.
 */
void *memcpy(void *dst, const void *src, size_t n)
{
    char *d = dst;
    const char *s = src;

	// Copies contents for n bytes
    while (n--) {
        *d++ = *s++;
    }
    return dst;
}

/* This function copies 'n' bytes of data from the memory
 * area 'src' to the memory area 'dst'. This function takes
 * three parameters: dst is a pointer array to be filled, val is an
 * integer value, and n is an undigned size_t integer that specifies
 * the number of bytes in memory that should be filled with val.
 */
void *memset(void *dst, int val, size_t n)
{
   char least_byte = val; // least significant byte of val
   char *d = dst;

   // Copies least significant byte of val for n bytes of 
   // the destination array
   while (n--) {
		d[n] = least_byte;
   }		
   return dst;
}

/* This function calculates the length of a string and returns this
 * as a size_t. It takes one paramater which is a char* string array
 * that the length of is calculated by this function.
 */
size_t strlen(const char *str)
{
    size_t n = 0;
	// Increment counter until end of string reached
    while (str[n] != '\0') {
        n++;
    }
    return n;
}

/* This function lexicographically compares two null-terminated strings
 * 's1' and 's2' which are parameters to the function. The function returns
 * the ASCII value of the first differing character or 0 if at the end of both
 * strings.
 */
int strcmp(const char *s1, const char *s2)
{
    int i = 0;

	// Initializes a and b
	char a = s1[0];
	char b = s2[0];

	// While neither are nullptr
	while (s1[i] && s2[i]) {
		a = s1[i]; 
		b = s2[i];
		// if first differing character is reached
		if (a - b > 0 || a - b < 0) {
				return (a - b);
		}
		i++;
	}
    return (s1[i] - s2[i]); // statement for if one or more nullptrs reached
}

/* This function performs size-bounded string concatenation. The 
 * function appends null-terminated string 'src' to the end of 'dst'
 * and null terminates the resulting string in 'dst'. The amount of 'src' that
 * is appended is determined by 'dstsize', a parameter that describes the 
 * amount of memory allocated to 'dst'. The function returns the final size of
 * dst if there were enough space to append all of src.
 */
size_t strlcat(char *dst, const char *src, size_t dstsize)
{
    // dstsize not correct or dst not proper string
    if (strlen(dst) > dstsize || dstsize == 0) {
		return dstsize + strlen(src);
	}
	char *end = dst;
	// Identify end of dst (where null ptr is)
	while (*end) {
		end++;
	}
	size_t total = strlen(dst) + strlen(src); // Size of what should be written to dst
	size_t remaining = dstsize - strlen(dst) - 1; // remaining space in dst
	// Only add the length of src that fits in the rest of dst
	memcpy(end, src, remaining);
	end[remaining] = '\0';
	return total;
}


/* This function takes an input of a string that is either a hex 
 * number beginning with "0x" or a decimal number and returns it
 * in integer format. The function also takes a const double pointer
 * to a char as a parameter and sets this to equal the end of the hex
 * or decimal number in the input string if the **endptr is not set
 * to NULL.
 */
unsigned int strtonum(const char *str, const char **endptr)
{
    // This accounts for the empty string
	if (*str == '\0') {
	    // Set endptr to nullptr on empty string
		if (endptr) {
		    *endptr = &str[0];
		}
		return 0;
	}
	int length = 0; // Keeps track of end length of valid hex/decimal number


	int result = 0;
	int is_hex = 0; // functions a bool value
	int index = 0;

	if (strlen(str) >= 2) { // Can only be hex if starts with "0x"
		if (str[0] == '0' && str[1] == 'x') {
				is_hex = 1; // Bool value is true, string interpreted as a hex number
				length = 2; // length of string is at least 2

				// Finds end of hex value by looking for nullptr and checking that end pointer
				// is an ASCII value for 0 to 9, a to f, or A to F
				while (str[length] && ((48 <= str[length] && str[length] <= 57) || (97 <= str[length] && str[length] <= 102) 
				|| (65 <= str[length] && str[length] <= 70))) { 
					length++; // end length of valid hex number
				}

				index = length - 1; // Starts at end of hex value
				int i = 0;
				int number = 0;

				// Moves backwards through string of numbers
				// to convert to decimal
				while (str[index] != 'x') {
				    // ASCII char range for a to f, converting to actual decimal integer
				    if (97 <= str[index] && str[index] <= 102) {
						number = str[index] - 87;
					}
					// ASCII char range for A to F
				    else if (65 <= str[index] && str[index] <= 70) {
						number = str[index] - 55;
					}
					// ASCII char range for 0 to 9
				    else if (48 <= str[index] && str[index] <= 57) {
						number = str[index] - 48;
					}
					// Multiplies number by 16 depending on digit number
					// for base 16 to base 10 conversion
					// Multiplies units digit by 1, tens by 16, hundreds by 16^2 etc.
				    for (int j = 0; j < i; j++) {
						number *= 16;
					}
				    result += number;
					index--;
					i++;
				}
		}
	}
	if (is_hex == 0) { // if not hex, then maybe base 10 decimal

	    // Finds end of value by looking for end pointer to looking for invalid
		// digit (so ASCII value not in range for characters 0 to 9).
		while (str[length] && (48 <= str[length] && str[length] <= 57)) {
		    //end++;
			length++; // end length of valid decimal number
		} 
		index = length - 1; // Starts at end of decimal value
		int i = 0;
		int number = 0;

		// Moves backwards through string of numbers to convert to decimal
		// Breaks out of loop once beginning of string reached
		while (48 <= str[index] && str[index] <= 57 && index >= 0) {
		    number = str[index] - 48;

		    // Multiplies number by 10 depending on digit number
			// Multiplies units by 1, tens by 10, hundreds by 100 etc
			for (int j = 0; j < i; j++) {
				number *= 10;
			}
			result += number;
			index--;
			i++;
		} 

	}
	if (endptr) {
		*endptr = &str[length];	// Address of first invalid hex/decimal digit in str
	}
	return result;
}
