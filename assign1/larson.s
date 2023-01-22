/*
 * LARSON SCANNER
 *
 * Currently this code is the blink code copied from lab 1.
 *
 * Modify the code to implement the larson scanner for assignment 1.
 *
 * Make sure to use GPIO pins 20-23 (or 20-27) for your scanner.
 */

.equ DELAY, 0x3F0000
.equ PINS, 0b100 // number of pins we're using is 8

mov r2, #PINS // store number of pins in r2
// configure GPIO20 for output
mov r1, #0 // stores binary value for accessing GPIO 20 in r1
pins_output:
    subs r2, #1 // subtracts one from pins configured  counter
    mov r1, r1, LSL #3 // stores binary value for accessing next GPIO pin in r1
   // mov r1, #(1<<3)
    add r1, r1, #1 // makes it input pin
    bne pins_output // loop continues until all pins have been configured

ldr r0, FSEL2 // stores FSEL2 address in r0
str r1, [r0] // makes GPIO 20 an output pin

mov r3, #PINS
// set bit 20
mov r1, #(1<<20)

loop: 

scanner_right:

    subs r3, #1
    
    // set GPIO 20 high
    ldr r0, SET0
    str r1, [r0] 

    // delay
    mov r2, #DELAY
    wait1:
        subs r2, #1
        bne wait1

    // set GPIO 20 low
    ldr r0, CLR0
    str r1, [r0]

    // delay
    mov r2, #DELAY
    wait2:
        subs r2, #1
        bne wait2

    mov r1, r1, LSL #1
    //mov r1, #(1<<21)
    
bne scanner_right

b loop
 
 FSEL0: .word 0x20200000
 FSEL1: .word 0x20200004
 FSEL2: .word 0x20200008
 SET0:  .word 0x2020001C
 SET1:  .word 0x20200020
 CLR0:  .word 0x20200028
 CLR1:  .word 0x2020002C
 
 
 

 
 
 

 
 
 
 
 

 
 
 
b


