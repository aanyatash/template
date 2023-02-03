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
   return NULL;
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
		//int remaining_concat = remaining < 0 ? -remaining : remaining;
		end[remaining] = 0;
		while (remaining--) {
		    end[remaining] = src[remaining];
		}

	}
    if (remaining >= strlen(src)) {
		int i = 0;
		while (src[i]) {
		    end[i] = src[i];
			i++;
		}
		end[i] = 0;
	}
	return strlen(dst);
}

unsigned int strtonum(const char *str, const char **endptr)
{
    /* TODO: Your code here */
    return 0;
}
