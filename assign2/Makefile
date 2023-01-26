# Programs built by this makefile
RUN_PROGRAM   = clock.bin
TEST_PROGRAM  = test_gpio_timer.bin

MY_MODULE_SOURCES = gpio.c timer.c

PROGRAMS      = $(RUN_PROGRAM) $(TEST_PROGRAM)

all: $(PROGRAMS)

# Flags for compile and link
CFLAGS	= -I$(CS107E)/include -Og -g -std=c99 $$warn $$freestanding
LDFLAGS	= -nostdlib -T memmap -L$(CS107E)/lib
LDLIBS 	=

# Common objects for the programs built by this makefile
SOURCES = start.s cstart.c $(MY_MODULE_SOURCES)
OBJECTS = $(addsuffix .o, $(basename $(SOURCES)))

# Rules and recipes for all build steps

# Extract raw binary from elf executable
%.bin: %.elf
	arm-none-eabi-objcopy $< -O binary $@

# Link program executable from program.o and all common objects
%.elf: $(OBJECTS) %.o
	@echo arm-none-eabi-gcc $(LDFLAGS) $^ $(LDLIBS) -o $@
	@$(CS107E)/bin/link-filter arm-none-eabi-gcc $(LDFLAGS) $^ $(LDLIBS) -o $@

# Compile C source to object file
%.o: %.c
	arm-none-eabi-gcc $(CFLAGS) -c $< -o $@

# Assemble asm source to object file
%.o: %.s
	arm-none-eabi-as $< -o $@

# Disassemble object file to asm listing
%.list: %.o
	arm-none-eabi-objdump --no-show-raw-insn -d $< > $@

# Build and run the application binary
run: $(RUN_PROGRAM)
	rpi-run.py $<

# Build and run the test binary
test: $(TEST_PROGRAM)
	rpi-run.py $<

# Sample application program, just for fun
larson: larson.bin
	rpi-run.py $<

# Remove all build projects
clean:
	rm -f *.o *.bin *.elf *.list

# this rule will provide better error message when
# a source file cannot be found (missing, misnamed)
$(SOURCES) $(PROGRAMS:.bin=.c):
	$(error cannot find source file `$@` needed for build)

.PHONY: all clean run test
.PRECIOUS: %.elf %.o

# disable built-in rules (they are not used)
.SUFFIXES:

export warn = -Wall -Wpointer-arith -Wwrite-strings -Werror \
              -Wno-error=unused-function -Wno-error=unused-variable \
              -fno-diagnostics-show-option
export freestanding = -ffreestanding -nostdinc \
                      -isystem $(shell arm-none-eabi-gcc -print-file-name=include)

define CS107E_ERROR_MESSAGE
ERROR - CS107E environment variable is not set.

Review instructions for properly configuring your shell.
https://cs107e.github.io/guides/install/userconfig#env

endef

ifndef CS107E
$(error $(CS107E_ERROR_MESSAGE))
endif
