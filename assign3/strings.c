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
    if (strlen(dst) > dstsize) {
		return dstsize + strlen(src);
	}
	char *end = dst;
	while (*end) {
		end++;
	}
	size_t total = strlen(dst) + strlen(src);
	size_t remaining = dstsize - strlen(dst) - 1;
	// if more than 0 add whole thing and nullptr
	// if less than 0, add strlen - abs
	if (remaining < strlen(src)) {
		memcpy(end, src, remaining);
		end[remaining] = '\0';
	}
    else if (remaining >= strlen(src)) {
		memcpy(end, src, remaining);
		end[remaining] = '\0';
    }
	return total;
}



unsigned int strtonum(const char *str, const char **endptr)
{
    // ACCOUNT FOR STR BEING JUST NULLPTR OR EMPTY
	if (*str == '\0') {
		if (**endptr != '\0') {
		    *endptr = &str[0];
		}
		return 0;
	}
	int length = 0;
	char buf[strlen(str)];
	memset(buf, '\0', sizeof(buf));
	memcpy(buf, str, sizeof(buf));
	char *end = buf;
	int result = 0;
	int is_hex = 0;
	if (strlen(str) >= 2) {
		// Only for hex numbers
		if (str[0] == '0' && str[1] == 'x') {
				is_hex = 1;
				length = 2;
				end += 2;
				while (*end && ((48 <= *end && *end <= 57) || (97 <= *end && *end <= 102) 
				|| (65 <= *end && *end <= 70))) { 
					end++;
					length++;
				}
				char *hex = end - 1;
				int i = 0;
				int number = 0;
				while (*hex != 'x') {
				    if (97 <= *hex && *hex <= 102) {
						number = *hex - 87;
					}
				    else if (65 <= *hex && *hex <= 70) {
						number = *hex - 55;
					}
				    else if (48 <= *hex && *hex <= 57) {
						number = *hex - 48;
					}
				    for (int j = 0; j < i; j++) {
						number *= 16;
					}
				    result += number;
					hex--;
					i++;
				}
		}
	}
	if (is_hex == 0) {
		while (*end && (48 <= *end && *end <= 57)) {
		    end++;
			length++;
		} 
		char *dec = end - 1;
		int i = 0;
		int number = 0;
		while (48 <= *dec && *dec <= 57) {
		    number = *dec - 48;
			for (int j = 0; j < i; j++) {
				number *= 10;
			}
			result += number;
			if (str == dec) {
				break;
			}
			dec--;
			i++;
		} 

	}
	// do same for decimal numbers
	if (**endptr != '\0') {
		*endptr = &str[length];	
	}
	return result;
}
