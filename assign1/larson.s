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
.equ PINS, 0b0111  // number of pins we're using is 8

mov r2, #PINS // store number of pins in r2
// configure GPIO20 for output
mov r1, #0 // stores binary value for accessing GPIO 20 in r1
pins_output:
    subs r2, #1 // subtracts one from pins configured  counter
    mov r1, r1, LSL #3 // stores binary value for accessing next GPIO pin in r1
    add r1, r1, #1 // makes it input pin
    bne pins_output // loop continues until all pins have been configured

ldr r0, FSEL2 // stores FSEL2 address in r0
str r1, [r0] // makes GPIO 20-27 an output pin

mov r1, #(1<<20)
mov r3, #0x8

looptest:

//mov r1, #(1<<20)
//mov r3, #0x8


testright:
    subs r3, #1
    ldr r0, SET0
    str r1, [r0] 

    // delay
    mov r2, #DELAY
    waittest:
        subs r2, #1
        bne waittest

    // set GPIO 20 low
    ldr r0, CLR0
    str r1, [r0]
    mov r1, r1, LSL #1

bne testright

//mov r1, #(1<<20)
mov r4, #0x8

testleft:
    subs r4, #1
    ldr r0, SET0
    str r1, [r0] 

    // delay
    mov r2, #DELAY
    waittestleft:
        subs r2, #1
        bne waittestleft

    // set GPIO 20 low
    ldr r0, CLR0
    str r1, [r0]
    mov r1, r1, LSR #1

bne testleft

//mov r3, #0x8

b looptest

mov r3, #PINS
sub r3, r3, #1
// set bit 20
mov r1, #(1<<20)

    // set GPIO 20 high
    ldr r0, SET0
    str r1, [r0] 

    // delay
    mov r2, #DELAY
    wait_initial:
        subs r2, #1
        bne wait_initial

    // set GPIO 20 low
    ldr r0, CLR0
    str r1, [r0]

    // delay
    mov r2, #DELAY
    wait_initial2:
        subs r2, #1
        bne wait_initial2


loop: 

scanner_right:

    subs r3, #1
    mov r1, r1, LSL #1

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

    //mov r1, r1, LSL #1
    //mov r1, #(1<<21)
 
    // delay
    mov r2, #DELAY
    wait2:
        subs r2, #1
        bne wait2

   
    bne scanner_right

mov r3, #PINS
sub r3, r3, #1
//mov r1, r1, LSR #1

scanner_left:

    subs r3, #1
    mov r1, r1, LSR #1
    
    // set GPIO 20 high
    ldr r0, SET0
    str r1, [r0] 

    // delay
    mov r2, #DELAY
    wait1_left:
        subs r2, #1
        bne wait1_left

    // set GPIO 20 low
    ldr r0, CLR0
    str r1, [r0]

    // delay
    mov r2, #DELAY
    wait2_left:
        subs r2, #1
        bne wait2_left


  //  mov r1, r1, LSR #1
    //mov r1, #(1<<21)
    
   bne scanner_left

mov r3, #PINS
sub r3, r3, #1
// set bit 20
//mov r1, #(1<<20)

b loop
 
 FSEL0: .word 0x20200000
 FSEL1: .word 0x20200004
 FSEL2: .word 0x20200008
 SET0:  .word 0x2020001C
 SET1:  .word 0x20200020
 CLR0:  .word 0x20200028
 CLR1:  .word 0x2020002C
 
 
 

 
 
 

 



