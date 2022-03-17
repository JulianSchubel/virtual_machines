#ifndef LC3_CONDITION_FLAGS_H
#define LC3_CONDITION_FLAGS_H

//CONDITION FLAGS
//3 condition flags
enum
{
	FL_POS = 1 << 0,	//positive 	1	0000 0001
	FL_ZER = 1 << 1,	//zero		2	0000 0010
	FL_NEG = 1 << 2		//Negative	4	0000 0100
};

#endif //LC3_CONDITION_FLAGS_H
