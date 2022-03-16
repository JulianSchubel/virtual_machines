#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

/* unix */
#include <unistd.h>
#include <fcntl.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/mman.h>

//MAIN MEMORY
//65,536 memory locations
uint16_t memory[UINT16_MAX];	//Ensure that the array length is the maximum representable value of 16-bit integer.

//REGISTERS	
//10 16-bit Registers
//8 General purpose registers 	(R0-R7)
//1 Program Counter register 	(PC)
//1 Condition Flag register 	(COND)
enum
{
	R_R0 = 0,
	R_R1,
	R_R2,
	R_R3,
	R_R4,
	R_R5,
	R_R6,
	R_R7,
	R_PC,		//Program counter
	R_COND,		//Stores flags providing information about the most recently executed calculation. Allows programs to check logical conditions
	R_COUNT 	//Total registers
};

uint16_t registers[R_COUNT];

//OPCODES
//16 opcodes in lc-3
enum
{
	OP_BR = 0,	//branch
	OP_ADD,		//add
	OP_LD,		//load
	OP_ST,		//store
	OP_JSR,		//jump register
	OP_AND,		//bitwise and
	OP_LDR,		//load register
	OP_STR,		//store register
	OP_RTI,		//unused
	OP_NOT,		//bitwise not
	OP_LDI,		//load indirect
	OP_STI,		//store indirect
	OP_JMP,		//jump
	OP_RES,		//reserved (unused)
	OP_LEA,		//load effective address
	OP_TRAP		//execute trap
};


//CONDITION FLAGS
//3 condition flags
enum
{
	FL_POS = 1 << 0,	//positive 	1	0000 0001
	FL_ZER = 1 << 1,	//zero		2	0000 0010
	FL_NEG = 1 << 2		//Negative	4	0000 0100
};		

int main(int argc, char * argv[])
{
	
	return 0;
}
