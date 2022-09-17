#ifndef MEMORY_MAPPED_REGISTERS_H
#define MEMORY_MAPPED_REGISTERS_H

/* Special registers not accessible from the standard register table */
enum {
    MMR_KBSR = 0xFE00 /* Keyboard Status Register */
    MMR_KBDR = 0xFE02 /* Keyboard Data Register */
}
#endif //MEMORY_MAPPED_REGISTERS_H
