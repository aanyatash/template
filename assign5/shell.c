/* Name: Aanya Tashfeen
 * Filename: shell.c
 * This file contains the code for a command line interface for the Pi.
 * The commands it can do are peek, poke, help, echo, and reboot.
 */
#include "shell.h"
#include "shell_commands.h"
#include "uart.h"
#include "printf.h"
#include "strings.h"
#include "malloc.h"
#include "pi.h"

#define LINE_LEN 80

static input_fn_t shell_read;
static formatted_fn_t shell_printf;

static const unsigned int commands_size = 5;

// NOTE TO STUDENTS:
// Your shell commands output various information and respond to user
// error with helpful messages. The specific wording and format of
// these messages would not generally be of great importance, but
// in order to streamline grading, we ask that you aim to match the
// output of the reference version.
//
// The behavior of the shell commands is documented in "shell_commands.h"
// https://cs107e.github.io/header#shell_commands
// The header file gives example output and error messages for all
// commands of the reference shell. Please match this wording and format.
//
// Your graders thank you in advance for taking this care!

// each command_t struct holds name, usage, description, and function ptr
static const command_t commands[] = {
    {"help", "help [cmd]",  "print command usage and description", cmd_help},
    {"echo", "echo [args]", "print arguments", cmd_echo},
	{"reboot", "reboot", "reboot the Raspberry Pi", cmd_reboot},
	{"peek", "peek [addr]", "print contents of memory at address", cmd_peek},
	{"poke", "poke [addr] [val]", "store value into memory at address", cmd_poke}
};


/* This function takes in two arguments:
 * 1) An integer size of array 'argc'
 * 2) An array of strings 'argv' that it will print out. 
 * The first argument in the array is the name of the function, which it will not print. 
 * The function returns 0 to indicate success.
 */
int cmd_echo(int argc, const char *argv[])
{
    for (int i = 1; i < argc; ++i) // don't print first arg, so i = 1
        shell_printf("%s ", argv[i]); // print every argument
    shell_printf("\n");
    return 0;
}

/* This function takes in two arguments:
 * 1) An integer size of array 'argc'
 * 2) An array of strings 'argv' (the function ignores the first arg, as this is "help")
 * The function will print out the usage and description of the command named in the argument.
 * If there are no functions named, it will print out the usage and description of all commands.
 * The first argument in the array is the name of the function, which it will not print. 
 * The function returns 0 to indicate success and -1 to indicate an error when the command
 * does not exist.
 */
int cmd_help(int argc, const char *argv[])
{
    // Iterate thorugh commands to find string matched with argv[1]
	for (int i = 0; i < commands_size; i++) {
	    if (argc == 1) { // prints out all command help lines, if no command named
		    shell_printf("Usage: %s\n", commands[i].usage);
			shell_printf("Description: %s\n", commands[i].description);
			if (i == commands_size - 1) { // return 0 to indicate success, once everything printed
				return 0;
			}
		}
		if (argc != 1) { // Command is named
		    if (strcmp(argv[1], commands[i].name) == 0) { // So, only print once command found in commands table
		        shell_printf("Usage: %s\n", commands[i].usage);
			    shell_printf("Description: %s\n", commands[i].description);
			    return 0;
		    }
		}
	}
    // No command found, indicate error/failure
    shell_printf("error: no such command '%s'\n", argv[1]);
    return -1;
}

/* This function takes in two arguments:
 * 1) An integer size of array 'argc'
 * 2) An array of strings 'argv' 
 * The function ignores all arguments and reboots the Pi. 
 */
int cmd_reboot(int argc, const char *argv[]) {
    shell_printf("Rebooting Pi, see ya at the bootloader!");
    uart_putchar(EOT);
    pi_reboot();
	return 0;
}


/* This function takes in two arguments:
 * 1) An integer size of array 'argc'
 * 2) An array of strings 'argv' (the function ignores the first arg, as this is "peek")
 * The specified memory address is the second element in 'argv'.
 * The function will print out the 4-byte value stored at the specified memory address.
 * If this is not a valid memory address, a descriptive error message is printed.
 * The function returns 0 to indicate success and -1 to indicate an error.
 */
int cmd_peek(int argc, const char *argv[]) {
    if (argc == 1) {
		shell_printf("error: peek expects 1 argument [addr]\n");
		return -1;
	}
	// Convert string to a number, so it can later be used as a memory address
    unsigned int result = strtonum(argv[1], NULL);
    if (result == 0) { // strtonum returns 0 to indicate invalid number
		shell_printf("error: peek cannot convert '%s'\n", argv[1]);
		return -1;
	}
	// Checks if memory address is 4-byte aligned
	if (result % 4 != 0) {
		shell_printf("error: peek address must be 4-byte aligned\n");
		return -1;
	}
	// Type cast number to memory address using an unsigned int ptr
	unsigned int *ptr = (unsigned int *) result;
	shell_printf("%p:	%08x\n", ptr, *ptr);
	return 0;
}

/* This function takes in two arguments:
 * 1) An integer size of array 'argc'
 * 2) An array of strings 'argv' (the function ignores the first arg, as this is "poke")
 * The specified memory address is the second element in 'argv'.
 * The value to store at that memory address is the third element in 'argv'.
 * The function will store the specified value at the specified memory address.
 * If this is not a valid memory address or not a valid number, a descriptive error message is printed.
 * The function returns 0 to indicate success and -1 to indicate an error.
 */
int cmd_poke(int argc, const char *argv[]) {
    // Too few arguments provided
    if (argc < 3) {
		shell_printf("error: poke expects 2 arguments [addr] and [val]\n");
		return -1;
	}
	// Converts string to a number, so it can be used as a memory address
	unsigned int address = strtonum(argv[1], NULL);
	if (address == 0) { // Invalid number given, as returned by strtonum
		shell_printf("error: poke cannot convert '%s'\n", argv[1]);
		return -1;
	}
	if (address % 4 != 0) { // Checks if address is 4-byte aligned
		shell_printf("error: poke address must be 4-byte aligned\n");
		return -1;
	}
	// Converts given value to a number, so it can be stored in the address
	unsigned int val = strtonum(argv[2], NULL);
	if (strcmp(argv[2], "0") != 0 && val == 0) {
		shell_printf("error: poke cannot convert '%s'\n", argv[2]);
		return -1;
	}
	// Type cast address integer to a pointer, so a value can be stored at it
	unsigned int *ptr = (unsigned int *) address; 
	*ptr = val;
    return 0;
}

/* This function initializes the shell. Tt takes two function pointers
 * as arguments:
 * 1) read_fn reads the input
 * 2) print_fn displays the output
 * The client chooses where the input is read from and where the output
 * is written to.
 */
void shell_init(input_fn_t read_fn, formatted_fn_t print_fn)
{
    shell_read = read_fn;
    shell_printf = print_fn;
}

/* This function sends the ASCII bell character over uart to
 * give an audio or visual beep depending on terminal settings.
 */
void shell_bell(void)
{
    uart_putchar('\a');
}

/* The function takes two arguments:
 * 1) an array of chars called buf to store a string
 * 2) an unsigned integer size_t called bufsize for max capacity
 * This function reads the input command line and stores the characters
 * in buf until the user hits return ('\n'). The function updates the display
 * to show the current line as the user edits. If the user writes more characters
 * than can be stored in buf, those excess characters are discarded and shell_bell()
 * is called. A null terminator is written to the end of buf.
 */
void shell_readline(char buf[], size_t bufsize)
{
    // can't be more than bufsize - 1
	// can't backspace if new line or nothing to backspace - use shell_bell
	int i = 0;
	buf[0] = '\0';
	unsigned char (*read)(void) = shell_read; // This is pointing to the input function
	int (*output)(const char *format, ...) = shell_printf; // Pointer to output function
	unsigned char character = read(); // Calling read() derefences it and stores it in character
	// Keeps reading the characters until the user hits return
	while (character != '\n') {
	    // If the user has typed enough characters to hit the buffer capacity,
		// keep discarding the excess characters, until backspace or return is hit
		while (i == bufsize - 1) {
		    shell_bell();
		    character = read(); // Keep reading input
			if (character == '\b') {
				break; // Break if backpsace as no longer at buffer capacity
			}
			if (character == '\n') { // If return key hit
				buf[i] = '\0'; // Null terminate for output function
				output("%c", '\n');
				return; // Stop adding to buffer once return is hit
			}
		}
		if (character == '\b') {
		    if (i == 0) { // If at the beginning of the line, nothing to backspace
				shell_bell();
			}
		    else {
			    output("%c %c", '\b', '\b'); // Writes whitespace over char to delete
				i = i - 1;
				buf[i] = '\0'; // Null terminate for output function
				output(buf+i);
			}
		}
		else {
	        buf[i] = character; // Add next typed character to buf
			buf[i+1] = '\0'; // Null terminate for output function
			output(buf+i);
		    i++;
		}
		character = read(); // Read next character for input line
	}
	buf[i] = '\0';
	output("%c", '\n');
}

/* This function takes one argument as a const char * calles 'line', which is the input string.
 * The function essentially parses the line to execute a command by adding each word in the line
 * to an array of strings called 'tokens'. The first token is the command and the function will lookup
 * the command using a command table and either execute it, or output an error message. The function
 * returns 0 to indicate success, or -1 to indicate failure.
 */
int shell_evaluate(const char *line)
{
    char** tokens = (char**) malloc(40 * sizeof(char*)); // this is an array of strings called tokens for string parsing

	int i = 0;
	int j = 0;

	// or you could find beginning and end of word
	// malloc that amount 
    // store pointer/pointer in tokens

	// keep iterating until null pointer reached

    // account for leading or trailing white space
	// don't add the space in between

	// Parses the line until the end is reached
	while (*line != '\0') {
	    // Only adds new token to array once end of word is reached, as indicated by a whitespace
		// or null character
	    if (*(line+j) == '\0' || *(line+j) == ' ' || *(line+j) == '\t' || *(line+j) == '\n') {
		    // Only add to tokens if this char isn't a leading space, so if j isn't 0
			if (j != 0) {
		       tokens[i] = (char*) malloc(j+1); // Allocate enough space for whole word and null character
			   memcpy(tokens[i], line, j);
			   tokens[i][j] = '\0'; // Ensure correctly terminated string
		       i++; // Next token to add to array
			   line = line + j; // Move j onto current character. This is either a whitespace or null char.
			   j = 0; // Parsing next word
			}
			if (*line) { // Only move onto first character of next word, if we haven't reached a null char.
			    // Moving pointer to next char ensures we don't add a leading space to our token
			    line++;
			}
		}
		else {
		    j++; // Move onto next character in word
		}
	}

	int tokens_size = i;

    // Iterate through commands table
	for (int i = 0; i < commands_size; i++) {
	    // Execute function once function that matches first token in array is found in commands table
		// tokens[0] is the command we want to execute
		if (strcmp(tokens[0], commands[i].name) == 0) {
		    // Create function pointer
		    int (*function)(int argc, const char *argv[]) = commands[i].fn;
			// Exectute command function and store result
	        int result = function(tokens_size,(const char**) tokens); 
		    // Ensure all memory is deallocated
	        for (int j = 0; j < tokens_size; j++) {
				free(tokens[j]);
		    }
			// Once command is executed, exit out of function
			return result;
		}
	}
	if (strcmp(tokens[0], "") == 0) {
		return -1;
	}
    // If we didn't return in for loop, command is not found
	shell_printf("error: no such command '%s'.\n", tokens[0]);
	// Ensure all memory is deallocated
	for (int i = 0; i < tokens_size; i++) {
	    free(tokens[i]);
	}
	free(tokens);
    return -1;

}

/* This function enters and infinite loop to read and evaluate an input and print an
 * ouput depending on the command inputted to the shell. This function never returns.
 */
void shell_run(void)
{
    shell_printf("Welcome to the CS107E shell. Remember to type on your PS/2 keyboard!\n");
    while (1)
    {
        char line[LINE_LEN];

        shell_printf("Pi> ");
        shell_readline(line, sizeof(line));
        shell_evaluate(line);
    }
}
