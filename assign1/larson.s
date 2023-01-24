/* Name: Aanya Tashfeen
 * Filename: larson.s
 *
 * LARSON SCANNER
 *
 * Implementation of the larson scanner extension for assignment 1.
 *
 * Use GPIO pins 20-27 for scanner
 */

.equ DELAY, 0x1F8000
.equ MID_BRIGHT_ON,0xFC
.equ MID_BRIGHT_OFF,0x762
.equ LOW_BRIGHT_ON,0x7E
.equ LOW_BRIGHT_OFF,0x7E0
.equ PINS, 0x8  // number of pins/LEDs we're using is 8

mov r2, #PINS // store number of pins in r2
// configure GPIO20 for output
mov r1, #0 // stores binary value for accessing GPIO 20 in r1
pins_output:
    subs r2, #1 // subtracts one from pins configured counter
    mov r1, r1, LSL #3 // stores binary value for accessing next GPIO pin in r1
    add r1, r1, #1 // makes it input pin
    bne pins_output // loop continues until all pins have been configured

ldr r0, FSEL2 // stores FSEL2 address in r0
str r1, [r0] // makes GPIO 20-27 an output pin

mov r1, #(1<<20) // sets voltage of leftmost pin GPIO as high 
mov r3, #PINS
mov r5, #(1<<22) // low bright
orr r5, r5, #(1<<18)
mov r6, #(1<<19) // mid bright
orr r6, r6, #(1<<21)
orr r1, r1, r5
orr r1, r1, r6

loop:

// moves through each LED from left to right
scanner_right:
    
    // set GPIO high
    mov r2, #DELAY
    wait1:
        mov r4, #LOW_BRIGHT_ON
        mov r7, #MID_BRIGHT_ON
        ldr r0, SET0
        str r1, [r0] // r1 is all of them added

	both_on_wait:
	    sub r7, r7, #1 // Middle brightness on counter
            sub r2, r2, #1 // Total delay counter
	    subs r4, #1 // Low brightness on counter
            bne both_on_wait

       	mov r4, #LOW_BRIGHT_OFF
	ldr r0, CLR0
	str r5, [r0] // Turn low brightness off part of duty cycle
	one_on_wait:
	    sub r4, r4, #1 // Low brightness off counter
	    sub r2, r2, #1 // Total delay counter
	    subs r7, #1 // Middle brightness on counter
	    bne one_on_wait

	ldr r0, CLR0
	str r6, [r0] // Turn middle brightness off
        both_off_wait:
	    sub r2, r2, #1 // Total delay counter
	    subs r4, #1 // Low brightness off counter
            bne both_off_wait

        subs r2, #1
        bpl wait1

    // set GPIO low
    ldr r0, CLR0
    str r1, [r0] // Turns brightest LED off

    mov r1, r1, LSL #1 // Moves on to pins to the right
    mov r5, r5, LSL #1
    mov r6, r6, LSL #1

    subs r3, #1 // Counter for each pin

bne scanner_right

mov r1, r1, LSR #2 // Skips over end LED when going back
mov r5, r5, LSR #2
mov r6, r6, LSR #2
mov r3, #PINS // Restart counter for pins
sub r3, r3, #1 // One less pin as skipping over end LED

// Moves through each LED from right to left
scanner_left:

    mov r2, #DELAY
    wait2:
        mov r4, #LOW_BRIGHT_ON
        mov r7, #MID_BRIGHT_ON
        ldr r0, SET0
        str r1, [r0] // r1 is all of them added

	both_on_wait2:
	    sub r7, r7, #1 // Middle brightness on counter
            sub r2, r2, #1 // Total delay counter
	    subs r4, #1 // Low brightness on counter
            bne both_on_wait2

       	mov r4, #LOW_BRIGHT_OFF
	ldr r0, CLR0
	str r5, [r0] // Turn low brightness off part of duty cycle
	one_on_wait2:
	    sub r4, r4, #1 // Low brightness off counter
	    sub r2, r2, #1 // Total delay counter
	    subs r7, #1 // Middle brightness on counter
	    bne one_on_wait2

	ldr r0, CLR0
	str r6, [r0] // Turn middle brightness off
        both_off_wait2:
	    sub r2, r2, #1 // Total delay counter
	    subs r4, #1 // Low brightness off counter
            bne both_off_wait2
    
        subs r2, #1 // delay counter
        bpl wait2

    // set GPIO low
    ldr r0, CLR0 
    str r1, [r0] // Turns brightest LED off

    mov r1, r1, LSR #1 // moves on to pins to the right
    mov r5, r5, LSR #1
    mov r6, r6, LSR #1

    subs r3, #1 // counter for each pin

bne scanner_left

mov r3, #PINS // resets pin counter
mov r1, r1, LSL #2 // skips over end LED when going back
mov r5, r5, LSL #2
mov r6, r6, LSL #2
sub r3, r3, #1 // One less pin as skipping over end LED

b loop


 FSEL0: .word 0x20200000
 FSEL1: .word 0x20200004
 FSEL2: .word 0x20200008
 SET0:  .word 0x2020001C
 SET1:  .word 0x20200020
 CLR0:  .word 0x20200028
 CLR1:  .word 0x2020002C
 
 



