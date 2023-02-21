#include "shell.h"
#include "shell_commands.h"
#include "uart.h"
#include "printf.h"
#include "strings.h"
#include "malloc.h"

#define LINE_LEN 80

static input_fn_t shell_read;
static formatted_fn_t shell_printf;

static unsigned int commands_size = 5;

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


static const command_t commands[] = {
    {"help", "help [cmd]",  "print command usage and description", cmd_help},
    {"echo", "echo [args]", "print arguments", cmd_echo},
	{"reboot", "reboot", "reboot the Raspberry Pi", cmd_reboot}//,
//	{"peek", "peek [addr]", "print contents of memory at address", cmd_peek},
//	{"poke", "poke [addr] [val]", "store value into memory at address", cmd_poke}
};

int cmd_echo(int argc, const char *argv[])
{
    for (int i = 1; i < argc; ++i)
        shell_printf("%s ", argv[i]);
    shell_printf("\n");
    return 0;
}

int cmd_help(int argc, const char *argv[])
{
    // iterate thorugh commands to find string matched with argv[1]
	for (int i = 0; i < commands_size; i++) {
	    if (argc == 1) {
		    shell_printf("Usage: %s\n", commands[i].usage);
			shell_printf("Description: %s\n", commands[i].description);
			if (i == commands_size - 1) {
				return 0;
			}
		}
		else if (strcmp(argv[1], commands[i].name) == 0) {
		    shell_printf("Usage: %s\n", commands[i].usage);
			shell_printf("Description: %s\n", commands[i].description);
			return 0;
		}
	}

    shell_printf("error: no such command '%s'\n", argv[1]);
    return -1;
}

int cmd_reboot(int argc, const char *argv[]) {
    shell_printf("Rebooting Pi, see ya at the bootloader!");
    uart_putchar(EOT);
	return 0;
    //pi_reboot();
}

void shell_init(input_fn_t read_fn, formatted_fn_t print_fn)
{
    shell_read = read_fn;
    shell_printf = print_fn;
}

void shell_bell(void)
{
    uart_putchar('\a');
}

void shell_readline(char buf[], size_t bufsize)
{
    // can't be more than bufsize - 1
	// can't backspace if new line or nothing to backspace - use shell_bell
	int i = 0;
	buf[0] = '\0';
	unsigned char (*read)(void) = shell_read; // this is pointing to the function
	int (*output)(const char *format, ...) = shell_printf;
	// we only want to call the function once and store it somewhere
	unsigned char character = read();
	while (character != '\n') { // calling the function dereferences it
		while (i == bufsize - 1) {
		    shell_bell();
		    character = read();
			if (character == '\b') {
				break;
			}
			if (character == '\n') {
				buf[i] = '\0';
				output("%c", '\n');
				return;
			}
		}
		if (character == '\b') {
		    if (i == 0) {
				shell_bell();
			}
		    else {
			    output("%c %c", '\b', '\b');
				i = i - 1;
				buf[i] = '\0';
				output(buf+i);
			}
		}
		else {
	        buf[i] = character;
			buf[i+1] = '\0';
			output(buf+i);
		    i++;
		}
		character = read();
	}
	buf[i] = '\0';
	output("%c", '\n');
}


int shell_evaluate(const char *line)
{
    char** tokens = (char**) malloc(40); // this is an array of pointers to tokens, [no of strings][max string size]

	int i = 0;
	int j = 0;

	// or you could find beginning and end of word
	// malloc that amount 
    // store pointer/pointer in tokens

	// keep iterating until null pointer reached

    // account for leading or trailing white space
	// don't add the space in between
	while (*line) {
	    if (*(line+j) == '\0' || *(line+j) == ' ' || *(line+j) == '\t' || *(line+j) == '\n') {
			if (j != 0) {
			   //tokens[i][j] = '\0';
		       tokens[i] = (char*) malloc(j+1);
			   memcpy(tokens[i], line, j);
			   //tokens[i] = (char*) dst;
		       i++;
			   line = line + j;
			   j = 0;
			}
			line++;
			//tokens[i][j] = *line;
		}
		else {
		    //tokens[i][j] = *line;
		    j++;
		    //line++;
		}
	}
	int tokens_size = i;
	//tokens[i][j] = '\0';

	for (int i = 0; i < commands_size; i++) {
		if (strcmp(tokens[0], commands[i].name) == 0) {
		    int (*function)(int argc, const char *argv[]) = commands[i].fn;
	        return function(tokens_size,(const char**) tokens);
		}
	}
    
    // line to parse
    // an array of char*
    // array of tokens
	// no ' ', '\t', '\n'
	// first token is command
    //return result;
	shell_printf("error: no such command '%s'.\n", tokens[0]);
    return -1;

}


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
