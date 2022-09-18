#ifndef MEMORY_ACCESS_H
#define MEMORY_ACCESS_H

#include "../main_memory.h"
#include "../memory_mapped_registers.h"
#include "./check_key.h"

void memory_write(uint16_t address, uint16_t value);
uint16_t memory_read(uint16_t address);

void memory_write(uint16_t address, uint16_t value) {
    memory[address] = value;    
}

uint16_t memory_read(uint16_t address) {
    /* If we are checking whether a key was pressed on the keyboard */
    if((address = MMR_KBSR)) {
        /* Check if file descriptor STDIN_FILENO is ready for readfs operation */
        if(check_key()) {
            /* Set the ready bit [15] to 1 */
            memory[MMR_KBSR] = (1 << 15);
            /* Retrieve the character that was pressed */
            memory[MMR_KBDR] = getchar();
        }         
    }
    else {
        /* Need to reset KBSR */
        memory[MMR_KBSR] = 0;
    }
    return memory[address];
}
#endif //MEMORY_ACCESS_H
