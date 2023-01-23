// File: codegen.c
// ---------------
// C examples that demo some interesting properties/transformations
// when C compiler generates ARM instructions.

// Part (a): arithmetic
// Study each function and the translation to ARM assembly.
// negate(): ARM has no negate instruction -- what is used instead?
// bitwise(): Note how constant expression is precomputed ("constant folding")
//            Which ARM instruction is used for this bitwise operation?
// arithmetic(): The first statement generates 3 assembly instructions, yet
//               second more complex line is just one. How/why does that happen?
int negate(int arg) {
    return -arg;
}

int bitwise(int arg) {
    return arg & ~((1 << 8) - 1);
}

int arithmetic(int arg) {
    int result = arg*22;
    result = arg - (result + 3*result);
    return result + 17;
}

// Part (b): if-else
// Branch instructions are used to make decisions about control flow.
// Look at the compiler's generated assembly for the code below.
// Which instruction evaluates the expression for the if?
// Which instruction set the condition codes? Which instruction reads
// the codes? How are branch instructions used to route execution?
// Compare the generated assembly for optimization level -Og to that
// for -O2? What is different?
int conditional(int arg) {
    int result = arg;

    if (arg % 2 == 0) {
        result++;
    } else {
        result--;
    }
    return result;
}

// Part (c): loops
// The function delay() is a C version of the delay loop we used in
// the blink program.  If set to optimize at level -Og, the compiler's
// generated assembly is a close match to our hand-written assembly.
// But at -O2 (aggressive optimization) it's a different story!
// Add volatile qualifier to the declaration of variable count.
// How does the generated assembly change now?
void delay(void) {
    int count = 0x3f00;
    while (--count != 0) ;
}

// The function wait_until_low() is a C version of
// "wait for button press" from lab1
// At -Og, the generated assembly repeatedly re-loads from LEV0
// and all is good. At -O2, it load once time and thereafter uses
// cached value in register.
// Is the proper fix to qualify `state` as volatile or `LEV0`?
// Try each one at a time to see the difference.

void wait_until_low(void) {
    unsigned int *LEV0  = (unsigned int *)0x20200034;
    unsigned int state = *LEV0;

    while (state & (1 << 10)) {
        state = *LEV0;
    }
}

// Part (d): pointers
// The function below accesses memory using pointer/array
// notation. The code is divided into pairs of statements
// which perform a similar operation, but have subtle differences
// (such as adding a typecast or operating on pointer of
// different pointee type). How do those differences affect
// the generated assembly?
int global;

void pointers(int n) {
    int *ptr = &global;
    char *cptr = (char *)&global;

    *ptr = 107;      // compare to next line
    *(char *)ptr = 55;

    ptr[5] = 13;     // compare to next line
    cptr[5] = 13;

    global = *(ptr + n);  // compare to next line
    global = ptr[n];
}


// Part (e): surprises
// precedence():  compiles to just return 7 -- why?

int precedence(int arg) {
    if (arg & 0x1 == 0) { // which precedence is higher: & vs. ==
        return 5;
    } else {
        return 7;
    }
}

// These last two exhibit "undefined behavior" (violates C language standard)
// C compiler does not reject outright, instead free to generate whatever asm
// not_initialized: declare a new variable but never initialize
// semi_initialize: what if only initialized on some paths?
int not_initialized(int arg) {
    int result;
    return result;
}

int semi_initialized(int arg) {
    int result;
    if (arg > 0)
        result = 5;
    return result;
}

void main(void) {}

