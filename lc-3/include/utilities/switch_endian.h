#ifndef LC3_SWITCH_ENDIAN_H
#define LC3_SWITCH_ENDIAN_H

#include <stdint.h>

/* 
    Endianness refers to how bytes are interpreted.
    Big-endian: Least significant byte at the largest memory address
    Little-endian: Least significant byte at the smallest memory address
*/
uint16_t switch_endian(uint16_t x)
{
	/* General algorithm for unsigned integers: x << (bit_size/2) | x >> (bit_size/2) */
	return ( x << 8 | x >> 8 );
}

#endif //LC3_SWITCH_ENDIAN_H
