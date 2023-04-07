str r1, [r2, r4]!
str r1, [r2], -r4
ldr r1, [r2, #16]
ldr r1, [r2, r3, lsl #2]
ldreqb r1, [r6, #5]

loop:
   add r1, r2, r3
   sub r2, r3, r4
   addeq r1, r2, r3
   addne r1, r2, r3
   add r1, r2, #0x28
   sub r2, r3, #0xFF0
   mov r0, r9, lsl #7
   mov r4, r8, lsr r6
   bne loop


ldr r0, [r1]
str r14, [r2]
ldrb r14, [r3, r2]!
strb r12, [r1, #-2]
rrx r1, r2
