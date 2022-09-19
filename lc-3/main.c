#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

/* nix headers */
#include <unistd.h>
#include <fcntl.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/mman.h>

/* Architecture definitions */
#include "./include/main_memory.h"
#include "./include/memory_mapped_registers.h"
#include "./include/registers.h"
#include "./include/opcodes.h"
#include "./include/condition_flags.h"
#include "./include/trap_codes.h"

/* Utility functions */
#include "./include/utilities/usage.h"
#include "./include/utilities/switch_endian.h"
#include "./include/utilities/read_image_file.h"
#include "./include/utilities/check_key.h"
#include "./include/utilities/memory_access.h"
#include "./include/utilities/terminal_io.h"
#include "./include/utilities/sign_extension.h"
#include "./include/utilities/update_condition_flags.h"

#define PROGRAM_START 0x3000

int main(int argc, char** argv)
{
    /* check if there are at least two command line arguments  */
    if(argc < 2)
    {    
        /* see include/utilities/usage.c  */
        usage(argc);
    }        
    /* read in the start of the image  */
    for( int i = 1; i < argc; ++i )
    {
        if( !read_image(argv[i]) )    
        {
            printf("Failed to load image: %s\n", argv[i]);
            exit(1);
        }
    }    

    /* Setup signal handler: Need terminal configuration to be reset on signal interrupt */
    signal(SIGINT, handle_interrupt);
    /* Alter input buffering */
    disable_input_buffering();

    /* Exactly one condition flag must be set at all times */
    registers[R_COND] = FL_ZER;    

    /* Instructions start at 0x3000 */
    /* Set the program counter to starting position by loading the address of the first instruction into the program counter */
    registers[R_PC] = PROGRAM_START;

    /* Sentinel value for the loop */
    int running = 1;

    while(running) 
    {
        /* Fetch */        
        uint16_t instruction = memory_read(registers[R_PC]++);

        /* Decode */
        /* Right shift to isolate the opcode; opcode is leftmost 4 bits */
        uint16_t opcode = instruction >> 12;

        /*  Identify the opcode and determine instruction operator and operands. Instructions are 16-bits wide */
        switch(opcode)
        {
            /* Execute */
            /* Register bit mask: 111 or 0x7 */
            case OP_ADD:
                {
                    /* Mode flag: Memory addressing mode */
                    /* 
                        Opcode: 0001

                        Bits [15:12]: (leftmost bits): store the opcode.
                        Bits [11:9]: store DR (Destination Register).
                        Bits [8:6]:    store SR1 (Source Register 1).     
                        Bits [5]: Mode flag (1 immediate mode, 0 register mode).
                        
                        If register mode (bit 5 = 0):
                        Bits [4:3]: Unused.
                        Bits [2:0]:    store SR2 (Source Register 2).
                            
                        If immediate mode (bit 5 = 1):
                        Bits [4:0]: imm5 field (5 bit value to be sign extended).

                        r0 = DR 
                        r1 = SR1
                        r2 = SR2
                    */

                    /* Isolate destination register */
                    uint16_t r0 = (instruction >> 9) & 0x7; 
                    /* Isolate source register 1 */
                    uint16_t r1 = (instruction >> 6) & 0x7;
                    /* Isolate the memory addressing mode */
                    uint16_t imm_flag = (instruction >> 5) & 0x1;  

                    if( imm_flag ) 
                    {
                        uint16_t imm5 = sign_extension(instruction & 0x1F, 5); 
                        registers[r0] = registers[r1] + imm5;
                    }
                    else
                    {
                        /* Isolate source register 2 */
                        uint16_t r2 = instruction & 0x7;
                        registers[r0] = registers[r1] + registers[r2];    
                    }

                    update_condition_flags(r0);
                }
                break;

            case OP_AND:
                {
                    /* 
                        Opcode: 0101

                        Bits [15:12]: opcode
                        Bits [11:9]: Destination Register
                        Bits [8:6]:    Source Register 1
                        Bits [5]: Mode flag.
                        
                        If register mode (bit 5 = 0):
                        Bits [4:3]: Unused
                        Bits [2:0]: Source Register 2

                        If immediate mode (bit 5 = 1):
                        Bits [4:0]: imm5 field.

                        r0 = DR 
                        r1 = SR1
                        r2 = SR2
                    */
                    uint16_t r0 = (instruction >> 9) & 0x7;
                    uint16_t r1 = (instruction >> 6) & 0x7;
                    uint16_t imm_flag = (instruction >> 5) & 0x1;

                    if( imm_flag )
                    {
                        uint16_t imm5 = sign_extension(instruction & 0x1F, 5);
                        registers[r0] = registers[r1] & imm5;
                    }
                    else
                    {
                        uint16_t r2 = instruction & 0x7;
                        registers[r0] = registers[r1] & registers[r2];
                    }

                    update_condition_flags(r0);
                }
                break;

            case OP_NOT:
                {
                    /*
                        Opcode: 1001

                        [15:12]: opcode
                        [11:9]: Destination register
                        [8:6]: Source Register
                        [5:0]: Unused
                    
                        r0 = DR
                        r1 = SR
                    */
                    uint16_t r0 = (instruction >> 9) & 0x7;
                    uint16_t r1 = (instruction >> 6) & 0x7;

                    registers[r0] = ~registers[r1];
                    update_condition_flags(r0);
                }
                break;

            case OP_BR:
                {
                    /* 
                        Opcode: 0000  
                        
                        [15:12]: opcode
                        [11]: negative condition code flag
                        [10]: zero condition code flag
                        [9]: positive condition code flag
                        [8:0]: 9 bit PC offset

                        FL_POS: 0001
                        FL_ZER: 0010
                        FL_NEG: 0100
                    */
                    uint16_t pc_offset_9 = sign_extension(instruction & 0x1FF, 9);

                    /* 
                        Condition is any flag set with no specific individual flag behaviour.
                        Handle the condition flags as a unit and & with registers[R_COND]. 
                    */    
                    uint16_t condition_flags = (instruction >> 9) & 0x7;    
                    
                    if(condition_flags & registers[R_COND])
                    {
                        registers[R_PC] += pc_offset_9; 
                    }
                }
                break;
                
            case OP_JMP:
                {
                    /*  
                        Unconditionally jump to the location specified by the base register
                        Also handles "return" (when PC is loaded with value from R7. That is, when r0 is 111)
                        Opcode: 1100

                        [15:12]: opcode
                        [11:9]: unused
                        [8:6]: Base Register 
                        [5:0]: unused
                    */
                    uint16_t r0 = (instruction >> 6) & 0x7;
                    registers[R_PC] = registers[r0];
                }
                break;

            case OP_JSR:
                {
                    /*  
                        Jump to Subroutine (JSR and JSRR)
                        Opcode: 0100
                        
                        [15:12]: opcode
                        [11]: subroutine address location flag
                        [10:0]: 11 PC offset
                    */

                    /* Save incremented program counter in R7: This is the linkage back to the calling routine */
                    registers[R_R7] = registers[R_PC];
                    uint16_t offset_flag = (instruction >> 11) & 1;
                    if(offset_flag) {
                        /* JSR: Subroutine address obtained from sign extending bits [10:0] and adding the value to PC */
                        /* Isolate 11 bit PC offset by bitwise AND using bit mask 0000 0111 1111 1111 or 0x7FF*/
                        uint16_t se_pc_offset_11 = sign_extension(instruction & 0x7FF, 11);
                        /* Add the sign extend PC offset to PC */
                        registers[R_PC] += se_pc_offset_11;
                    }
                    else {
                        /* JSRR: Subroutine address is obtained from the base register. Bits [8:6] */
                        uint16_t r0 = (instruction >> 6) & 0x7;
                        registers[R_PC] = registers[r0];
                    }
                }
                break;

            case OP_LD:
                {
                    /*  
                        Load: 
                            Address is computed by sign extending the 9 bit PC offset and adding to the PC
                            Contents of the resulting memory address loaded into the destination register
                            Limited to address offsets of 9 bits, i.e. "nearer" addresses

                            **Essentially functions like loading a non-register variable 

                        Opcode: 0010

                        [15:12]: opcode
                        [11:9]: Destination register
                        [8:0]: 9 bit PC offset
                    */

                    /* Retrieve the destination register */
                    uint16_t r0 = (instruction >> 9) & 0x7;
                    /* Isolate the 9 bit PC offset by bitwise AND using bit mask 0000 0001 1111 1111 or 0x1FF*/
                    uint16_t se_pc_offset_9 = sign_extension(instruction & 0x1FF, 9);
                    registers[r0] = memory_read(registers[R_PC] + se_pc_offset_9);
                    update_condition_flags(r0);
                }
                break;

            case OP_LDI:
                {
                    /*  
                        Load Indirect:
                            Address computed by sign extending the 9 bit PC offset and adding to the PC
                            Contents of the resulting memory address is the memory address of the data to be loaded into the DR
                            Allows for address offsets of 16 bits by storing the location holding the address in a "nearer" 9 bit address.

                            **Essentially functions like dereferencing a pointer variable.

                        Opcode: 1010

                        [15:12]: opcode
                        [11:9]: Destination Register
                        [8:0]: 9 bit PC offset
                    */
                    uint16_t r0 = (instruction >> 9) & 0x7;
                    uint16_t se_pc_offset_9 = sign_extension(instruction & 0x1FF, 9);
                    registers[r0] = memory_read(memory_read(registers[R_PC] + se_pc_offset_9));
                    update_condition_flags(r0);
                }
                break;

            case OP_LDR:
                {
                    /*  
                        Load Register:
                            Load from base register + offset
                            Address computed by sign-extending bits [5:0] and adding this value to the contents of the base register specified in bits [8:6]
                            Contents of the resulting address loaded into the DR specified at bits [11:9]

                        Opcode: 0110

                        [15:12]: opcode
                        [11:9]: Destination register
                        [8:6]: Base register
                        [5:0] 6 bit offset
                    */
                    uint16_t r0 = (instruction >> 9) & 0x7;
                    uint16_t r1 = (instruction >> 6) & 0x7;
                    uint16_t se_br_offset_6 = sign_extension(instruction & 0x3F, 6);
                    registers[r0] = memory_read(registers[r1] + se_br_offset_6);
                    update_condition_flags(r0);
                }
                break;

            case OP_LEA:
                {
                    /*  
                        Load Effective Address:
                            Load an address into a register
                            Address computed by sign-extending bits [8:0] and adding this to the PC.
                            The resulting address is loaded into the DR

                        Opcode: 1110

                        [15:12]: opcode
                        [11:9]: Destination register
                        [8:0]: 9 bit PC offset
                    */
                    uint16_t r0 = (instruction >> 9) & 0x7;
                    uint16_t se_pc_offset_9 = sign_extension(instruction & 0x1FF, 9);
                    registers[r0] = registers[R_PC] + se_pc_offset_9;
                    update_condition_flags(r0);
                }
                break;

            case OP_ST:
                {
                    /*  
                        Store:
                            Store the contents of a register in a memory location
                            The contents of the source register are stored in the memory location computed by sign extending bit [8:0] and adding the result to PC

                        Opcode: 0011

                        [15:12]:
                        [11:9]: Source register
                        [8:0]: 9 bit PC offset
                    */
                    uint16_t r0 = (instruction >> 9) & 0x7;
                    uint16_t se_pc_offset_9 = sign_extension(instruction & 0x1FF, 9);
                    memory_write(registers[R_PC] + se_pc_offset_9, registers[r0]);
                }
                break;

            case OP_STI:
                {
                    /*
                        Store Indirect

                            Contents of the soure register stored in the memory location computed by sign extending bits [8:0] and adding to the PC
                            The value at this address is the address in which to store the data 

                        Opcode: 1011

                        [15:12]: opcode
                        [11:9]: Source register
                        [8:0]: 9 bit PC offset
                    */
                    uint16_t r0 = (instruction >> 9) & 0x7;
                    uint16_t se_pc_offset_9 = sign_extension(instruction & 0x1FF, 9);
                    memory_write(memory_read(registers[R_PC] + se_pc_offset_9), registers[r0]);
                }
                break;

            case OP_STR:
                {
                    /*
                        Store Register:
                            Store in base register + offset
                            Contents of the source register stored in the address computed by sign extending bits [5:0] and adding to the base register bits [8:6]
                        Opcode: 0111

                        [15:12]: opcode
                        [11:9]: Source register
                        [8:6]: Base register
                        [5:0]: 6 bit BR offset
                    */
                    uint16_t r0 = (instruction >> 9) & 0x7;
                    uint16_t r1 = (instruction >> 6) & 0x7;
                    uint16_t se_br_offset_6 = sign_extension(instruction & 0x3F, 6);
                    memory_write(registers[r1] + se_br_offset_6, registers[r0]);
                }
                break;

            case OP_TRAP:
                {
                    /* 
                        Trap routines: predefined routines for performing common IO tasks 
                        R7 is loaded with the value of PC (enables return to the instruction following the trap routine call).
                        PC is then loaded with the trap routine starting address computed by zero extending the 8 bit trap vector [7:0].

                        Opcode: 1111

                        [15:12]: opcode
                        [11:8]: unused
                        [7:0]: 8 bit trap vector
                    */

                    /* Store the current PC for linkage back to calling routine */
                    registers[R_R7] = registers[R_PC];
                    switch (instruction & 0xFF)
                    {
                        case TRAP_GETC:
                            {
                                /*  
                                    Read a single character from the keyboard. 
                                    The character is not echoed onto the console.
                                    The ASCII code of the character is copied onto R0.
                                    The high 8 bits of R0 are cleared.
                                */
                                registers[R_R0] = ((uint16_t)getchar());
                                update_condition_flags(R_R0);
                            }
                            break;
                        case TRAP_OUT:
                            {
                                /*  
                                    Write a character in R0 [7:0] to the console
                                */
                                putc((char)registers[R_R0], stdout); 
                                fflush(stdout);
                            }
                            break;
                        case TRAP_PUTS:
                            {
                                /*  
                                    Write a string of characters to the console.
                                    Characters are contained in consecutive memory addresses, starting at address specified in R0.
                                    Occurrence of 0x0000 in memory location terminates.
                                */
                                /* R0 contains the array offset */
                                /* Note that unlike C where chars are a single byte, a char in LC3 is a 16 bit memory location */
                                uint16_t * c = memory + registers[R_R0];
                                while(*c) {
                                    putc((char)*c, stdout);
                                    ++c;
                                }
                                /* Flush stdout: i.e. force write of all bufferred user-space data for the stream */
                                fflush(stdout);
                            }
                            break;
                        case TRAP_IN:
                            {
                                /*  
                                    Print a prompt to the screen.
                                    Read a single character from the keyboard which is echoed to the console.
                                    The ASCII code for the character is copied to R0.
                                    The high 8 bits of R0 are cleared off.
                                */
                                printf("Enter a character: ");
                                char c = getchar();
                                putc(c, stdout);
                                registers[R_R0] = (uint16_t)c;
                                update_condition_flags(R_R0);
                            }
                            break;
                        case TRAP_PUTSP:
                            {
                                /*  
                                    Write a string of ASCII characters to the console.
                                    Characters in consecutive memory locations, 2 characters per location, starting at address specified in R).
                                    ASCII code contained in bits [7:0] written first.
                                    ASCII code contained in bits [15:8] written second.
                                    If an odd number of characters is to be written [15:8] has value 0x00
                                    Writing terminates if a value of 0x0000 is encountered.
                                */
                                /*  
                                    Integer numbers written as text are always represented most significant digit first in memory, think of a stack (stack grows down)
                                    This means we will need to switch from little-endian (x86) to big-endian (LC3), i.e. we need to swap the order of our characters
                                */
                                uint16_t * c = memory + registers[R_R0];
                                while(*c) {
                                    char c1 = (*c) & 0xFF;
                                    putc(c1, stdout);
                                    char c2 = (*c) >> 8;
                                    if(c2) {
                                        putc(c2, stdout);
                                    }
                                    ++c;
                                }
                                fflush(stdout);
                            }
                            break;
                        case TRAP_HALT:
                            /* Halt execution and print a message on the console. */
                            puts("HALT");
                            fflush(stdout);
                            running = 0;
                            break;
                    }
                }
                break;

            case OP_RES:
                /* Unused: fall through */
            case OP_RTI:
                /* Unused: fall through */
            default: 
                printf("Bad opcode, Aborting...\n");
                abort();
                break;
        }
    }

    /* shutdown */
    restore_input_buffering();
    return 0;
}
