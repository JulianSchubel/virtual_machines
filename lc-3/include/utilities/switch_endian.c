#ifndef LC3_SWITCH_ENDIAN_H
#define LC3_SWITCH_ENDIAN_H

#include <stdint.h>

uint16_t switch_endian(uint16_t x)
{
	/* General algorithm for unsigned integers: x << (bit_size/2) | x >> (bit_size/2) */
	return ( x << 8 | x >> 8 );
}

#endif //LC3_SWITCH_ENDIAN_H
