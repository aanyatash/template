My disassembler handles all data processing and branch/branch link instructions. 

For data processing instructions, the disassembler takes into account all of them and for shift
instructions, it uses instructions in the format mov, r1, r2, lsl #2. My disassembler handles
immediate values that need to be rotated, shift operations on registers, and shift operations on
numbers.

It also handles ldr, str, ldrb, and strb instructions. For these instructions, it takes into account
positive or negative offsets, the write-back bit, and pre and post indexing. It also handles shift by
a number and register shifts with logical shift operations. 

I tested my disassembler against the x/30i 0x8000 gdb command and all of the instructions appeared to be correct,
with any push pop instructions being just the encoding values as I did not implement this.

The instr.s assembly file I made can be used for reference on the types of instructions my disassembler should be able to 
handle. I used this file for testing.
