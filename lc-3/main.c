#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

/* *nix headers */
#include <unistd.h>
#include <fcntl.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/mman.h>

/* Architecture definitions */
#include "./include/main_memory.h"
#include "./include/registers.h"
#include "./include/opcodes.h"
#include "./include/condition_flags.h"

/* Utility functions */
#include "./include/utilities/usage.c"
#include "./include/utilities/switch_endian.c"
#include "./include/utilities/read_image_file.c"

#define PROGRAM_START 0x3000

uint16_t check_key()
{
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    return select(1, &readfds, NULL, NULL, &timeout) != 0;
}

//Set up terminal input
struct termios original_tio;

void disable_input_buffering()
{
    tcgetattr(STDIN_FILENO, &original_tio);
    struct termios new_tio = original_tio;
    new_tio.c_lflag &= ~ICANON & ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

void restore_input_buffering()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);
}

//Interrupt handling
void handle_interrupt(int signal)
{
    restore_input_buffering();
    printf("\n");
    exit(-2);
}

/* Whenever a value is written to a register, we need to update the condition flag to indicate its sign.  */
void update_condition_flags(uint16_t r)
{
	if(registers[r] == 0)
	{
		registers[R_COND] = FL_ZRO;
	}
	/* if sign bit is 1  */
	else if (registers[r] >> 15) 
	{
		registers[R_COND] = FL_NEG;
	}
	else
	{
		registers[R_COND] = FL_POS;
	}
}

/* 
		Provide sign extension:
 
		Used to increase the number of bits in a positional number reprentation while retaining the sign and value.
		If the number is positive padded with 0's, i.e. functionally, do nothing.
		If the number is negative padded with 1's, assuming two's complement.
*/
uint16_t sign_extension(uint16_t x, int bit_count)
{
	/* if the sign bit is negative  */
	if( (x >> (bit_count-1)) & 1 )
	{
		/* 
			Set the bits of x in a 16 bit, two's complement representation
			Each nibble 0xF is 1111
			0xFFFF is 1111 1111 1111 1111
		*/
		x |= (0xFFFF << bit_count);
	}
	return x;
}

/*int main(int argc, char * argv[])
{
	signal(SIGINT, handle_interrupt);
	disable_input_buffering();
	//Check that there is at least one argument
	//Exactly one condition flag should be set at any given time
	registers[R_COND] = FL_ZERO;


	restore_input_buffering();
	return 0;
}
*/

int main(int argc, char** argv)
{
	/* check if there are at least two command line arguments  */
	if(argc < 2)
	{	
		/* see include/utilities/usage.c  */
		usage(argc);
	}		
	/* read in the start of the image  */
	else
	{
		for( int i = 1; i < argc; ++i )
		{
			/* see include/utilities/read_image.c  */
			if( !read_image(argv[i]) )	
			{
				printf("Failed to load image: %s\n", argv[i]);
				exit(1);
			}
		}	
	}

	/* Exactly one condition flag must be set */
	registers[R_COND] = FL_ZER;	

	/* Instructions start at 0x3000 */
	/* Load the address of the first instruction to the program counter */
	registers[R_PC] = PROGRAM_START;

	/* Sentinel value for the loop */
	int running = 1;
	/*   */	
	while(running) 
	{
		/* Fetch the first instruction */		
		uint16_t instruction = mem_read(registers[R_PC]++);

		/* Right shift to isolate the opcode */
		uint16_t opcode = instruction >> 12;



		/*  Identify the opcode and determine instruction operator and operands.

			Instruction have both an opcode and parameters: 
			OPCODE: Type of operation to be performed.
			PARAMATERS: Inputs to the operation.
		*/

		switch(opcode)
		{
			case OP_ADD:
				{
					/* 
						Instructions are 16-bits wide:
						Bits [15:12]: (leftmost bits): store the opcode.
						Bits [11:9]: store DR (Destination Register).
						Bits [8:6]:	store SR1 (Source Register 1).	 
						Bits [5]: Mode flag (1 immediate mode, 0 register mode).
						
						If register mode:
						Bits [4:3]: Unused.
						Bits [2:0]:	store SR2 (Source Register 2).
							
						If immediate mode:
						Bits [4:0]: imm5 field (5 bit value to be sign extended).

						r0 = DR 
						r1 = SR1
						r2 = SR2
					*/

					uint16_t r0 = instruction >> 9 & 0x7; 
					uint16_t r1 = instruction >> 6 & 0x7;
					uint16_t imm_flag = instruction >> 5 & 0x1;  

					if( imm_flag ) 
					{
						uint16_t imm5 = sign_extension(instruction & 0x1F, 5); 
						registers[r0] = registers[r1] + imm5;
					}
					else
					{
						uint16_t r2 = instruction & 0x7;
						registers[r0] = registers[r1] + registers[r2];	
					}

					update_condition_flags(r0);
				}
				break;
			case OP_AND:
				break;
			case OP_NOT:
				break;
			case OP_BR:
				break;
			case OP_JMP:
				break;
			case OP_JSR:
				break;
			case OP_LD:
				break;
			case OP_LDI:
				break;
			case OP_LDR:
				break;
			case OP_LEA:
				break;
			case OP_ST:
				break;
			case OP_STI:
				break;
			case OP_STR:
				break;
			case OP_TRAP:
				break;
			case OP_RES:
				break;
			case OP_RTI:
				break;
			default: 
				/* bad opcode */
				break;
		}
	}
	/* shutdown */
	return 0;
}
