#ifndef LC3_SWITCH_ENDIAN_H
#define LC3_SWITCH_ENDIAN_H

#include <stdint.h>

/* 
    Endianness refers to how bytes are interpreted.
    Big-endian: Most significant byte in the smallest memory address, least significant byte in the largest memory address: that is the most significant value in the sequence is stored first.
    Little-endian: Least significant byte in the smallest memory address, most significant byte in the largest memory address: that is the least significant byte is stored first.
*/
uint16_t switch_endian(uint16_t x)
{
	return ( x << 8 | x >> 8 );
}

#endif //LC3_SWITCH_ENDIAN_H
