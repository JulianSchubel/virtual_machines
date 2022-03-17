#ifndef LC3_REGISTERS_H
#define LC3_REGISTERS_H

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

#endif //LC3_REGISTERS_H
