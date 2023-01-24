/* Name: Aanya Tashfeen
 * Filename: larson.s
 *
 * LARSON SCANNER
 *
 * Implementation of the larson scanner for assignment 1.
 *
 * Use GPIO pins 20-27 for scanner
 */

.equ DELAY, 0x3F00
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

#mov r1, #(1<<20) // sets voltage of leftmost pin GPIO as high 
#mov r3, #PINS
#//mov r5, #(1<<21)
#
#loopbright:
#
#lscanner_right:
#
#    // set GPIO high
#    //ldr r0, SET0
#    //str r1, [r0]
#
#    // delay
#    mov r2, #DELAY
#    lwait1:
#
#       mov r4, #LOW_BRIGHT_ON
#	ldr r0, SET0
#	str r1, [r0]
#	brighton:
#	    sub r2, r2, #20
#	    subs r4, #1
#            bne brighton
#
#	mov r4, #LOW_BRIGHT_OFF
#	ldr r0, CLR0
#	str r1, [r0]
#        brightoff:
#	    sub r2, r2, #20
#	    subs r4, #1
#            bne brightoff
#
#        subs r2, #20
#        blt lwait1
#
#    // set GPIO low
#    ldr r0, CLR0
#    str r1, [r0]
#    mov r1, r1, LSL #1 // moves on to pin to the right
#    subs r3, #1 // counter for each pin
#    bne lscanner_right
#
#mov r1, #(1<<20)
#mov r3, #PINS
#
#b loopbright





mov r1, #(1<<21) // sets voltage of leftmost pin GPIO as high 
mov r3, #PINS
mov r5, #(1<<20) // low bright
mov r6, #(1<<22) // mid bright
add r1, r1, r5
add r1, r1, r6

loop:

// moves through each LED from left to right
scanner_right:
    
    // set GPIO high
  //  ldr r0, SET0
  //  str r1, [r0]
   
    mov r2, #DELAY
    wait1:
        mov r4, #LOW_BRIGHT_ON
        mov r7, #MID_BRIGHT_ON
        ldr r0, SET0
        str r1, [r0]

	both_on_wait:
	    sub r7, r7, #1 // Mid on counter
//            sub r2, r2, #1 // Total delay counter
	    subs r4, #1 // Low on counter
            bne both_on_wait
       	mov r4, #LOW_BRIGHT_OFF
	//sub r1, r1, r5
	ldr r0, CLR0 // SAME REGISTER AS SET
	str r5, [r0] // turn low off
	one_on_wait:
	    sub r4, r4, #1 // Low off counter
//	    sub r2, r2, #1 // Total delay counter
	    subs r7, #1 // Mid on counter
	    bne one_on_wait
	//sub r1, r1, r6	
	ldr r0, CLR0
	str r6, [r0] // turn mid off
        both_off_wait:
//	    sub r2, r2, #1 // Total delay counter
	    subs r4, #1 // Low off counter
            bne both_off_wait
//	ldr r0, SET0
 //       str r1, [r0]
        subs r2, #1
        bne wait1

    // set GPIO low
    ldr r0, CLR0
    str r1, [r0]
    mov r1, r1, LSL #1 // moves on to pins to the right
    subs r3, #1 // counter for each pin

bne scanner_right

#mov r1, r1, LSR #1 // skips over end LED when going back
#mov r3, #PINS // restart counter for pins
#
#// moves through each LED from right to left
#scanner_left:
#
#    // set GPIO high
#    ldr r0, SET0
#    str r1, [r0] 
#
#    // delay
#    mov r2, #DELAY
#    wait2:
#        subs r2, #1
#        bne wait2
#
#    // set GPIO low
#    ldr r0, CLR0
#    str r1, [r0]
#    mov r1, r1, LSR #1 // moves onto pin to the left
#    subs r3, #1 // counter for each pin
#
#bne scanner_left
#
#mov r3, #PINS // resets pin counter
#mov r1, r1, LSL #1 // skips over end LED when going back

b loop


 FSEL0: .word 0x20200000
 FSEL1: .word 0x20200004
 FSEL2: .word 0x20200008
 SET0:  .word 0x2020001C
 SET1:  .word 0x20200020
 CLR0:  .word 0x20200028
 CLR1:  .word 0x2020002C
 
 



