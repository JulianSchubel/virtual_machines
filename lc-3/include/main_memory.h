#ifndef LC3_MAIN_MEMORY_H
#define LC3_MAIN_MEMORY_H

//MAIN MEMORY
//65,536 memory locations
uint16_t memory[UINT16_MAX];	//Array length is the maximum representable value of 16-bit integer i.e. 0xFFFF

/* 
    Memory locations 0x0000 through 0x00FF (256 total) are available to containt address for system calls specified by their corresponding trap vectors. 
    This region of memory is called the Trap Vector Table.
*/
#endif //LC3_MAIN_MEMORY_H
