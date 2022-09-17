#ifndef MEMORY_MAPPED_REGISTERS_H
#define MEMORY_MAPPED_REGISTERS_H

/* 
 Special registers not accessible from the standard register table 
 These registers have a designated address reserved in memory and are commonly used to interact with special hardware devices.
*/
enum {
    MMR_KBSR = 0xFE00, /* Keyboard Status Register: indicates whether a key has been pressed */
    MMR_KBDR = 0xFE02 /* Keyboard Data Register: contains the last key that was pressed */
};
#endif //MEMORY_MAPPED_REGISTERS_H
