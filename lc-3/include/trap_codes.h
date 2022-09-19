#ifndef LC3_TRAP_CODES_H
#define LC3_TRAP_CODES_H

/* Trap routines do not add extra functionality. They are simply a convenient way to perform a task (similar to OS system calls). */
/* Programs start at 0x3000 instead of 0x0 as the lower addresses are left empty to leave space for trap routine code. */

enum {
    TRAP_GETC = 0x20,   /* Get a character from the keyboard, not echoed onto the terminal */
    TRAP_OUT = 0x21,    /* Output a character */ 
    TRAP_PUTS = 0x22,   /* Output a word string */
    TRAP_IN = 0x23,     /* Get a character from the keyboard, echoed onto the terminal */
    TRAP_PUTSP = 0x24,  /* Output a byte string */
    TRAP_HALT = 0x25    /* Halt the program */
};

#endif //LC3_TRAP_CODES_H
