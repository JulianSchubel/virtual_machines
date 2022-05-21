#ifndef LC3_CONDITION_FLAGS_H
#define LC3_CONDITION_FLAGS_H

/*
	CONDITION FLAGS
	Stored in the R_COND register.
	Provide information about the most recently executed calculation.
	Exactly one conditition flag must be set at any given time.
*/

enum
{
	FL_POS = 1 << 0,	//positive 	1	0000 0001
	FL_ZER = 1 << 1,	//zero		2	0000 0010
	FL_NEG = 1 << 2		//Negative	4	0000 0100
};

#endif //LC3_CONDITION_FLAGS_H
