#ifndef LC3_OPCODES_H
#define LC3_OPCODES_H

//OPCODES
//16 opcodes in lc-3
enum
{
	OP_BR = 0,	//branch
	OP_ADD,		//add
	OP_LD,		//load
	OP_ST,		//store
	OP_JSR,		//jump to subroutine
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

#endif //LC3_OPCODES_H

