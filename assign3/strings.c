#include "strings.h"

void *memcpy(void *dst, const void *src, size_t n)
{
    /* Copy contents from src to dst one byte at a time */
    char *d = dst;
    const char *s = src;
    while (n--) {
        *d++ = *s++;
    }
    return dst;
}

void *memset(void *dst, int val, size_t n)
{
   char least_byte = val; // least significant byte of val
   char *d = dst;
   while (n--) {
		d[n] = least_byte;
   }		
   return dst;
}


size_t strlen(const char *str)
{
    /* Implementation a gift to you from lab3 */
    size_t n = 0;
    while (str[n] != '\0') {
        n++;
    }
    return n;
}

int strcmp(const char *s1, const char *s2)
{
    int i = 0;
	char a = s1[0];  // initializes a and b
	char b = s2[0];
	while (s1[i] && s2[i]) { // while neither are nullptr
		a = s1[i]; 
		b = s2[i];
		if (a - b > 0 || a - b < 0) {
				return (a - b);
		}
		i++;
	}
    return (s1[i] - s2[i]); // statement for if one or more nullptrs reached
}

size_t strlcat(char *dst, const char *src, size_t dstsize)
{
    if (strlen(dst) > dstsize) {
		return dstsize + strlen(src);
	}
	char *end = dst;
	while (*end) {
		end++;
	}
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
	return strlen(dst);
}



unsigned int strtonum(const char *str, const char **endptr)
{
    // ACCOUNT FOR STR BEING JUST NULLPTR OR EMPTY
	if (*str == '\0') {
		return 0;
	}
	int length = 0;
	char buf[strlen(str)];
	memcpy(buf, str, sizeof(buf));
	char *end = buf;
	int result = 0;
	int is_hex = 0;
	if (strlen(str) > 2) {
		// Only for hex numbers
		if (str[0] == '0' && str[1] == 'x') {
				is_hex = 1;
				end += 2;
				while (*end || (48 <= *end && *end <= 57) || (97 <= *end && *end <= 102) 
				|| (65 <= *end && *end <= 70)) { 
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
