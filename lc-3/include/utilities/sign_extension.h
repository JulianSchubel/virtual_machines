#ifndef SIGN_EXTENSION_H
#define SIGN_EXTENSION_H

/* 
    Provide sign extension:

    Used to increase the number of bits in a positional number reprentation while retaining the sign and value.
    If the number is positive padded with 0's, i.e. functionally, do nothing.
    If the number is negative padded with 1's, assuming two's complement.

    sign_extension: x is the value to be extended, bit_count is the number of bits in the value
*/

uint16_t sign_extension(uint16_t x, int bit_count)
{
    /* if the sign bit is negative  */
    if( (x >> (bit_count-1)) & 1 )
    {
        /* 
            Set the bits of x as a 16 bit two's complement representation
            Each nibble 0xF is 1111
            0xFFFF is 1111 1111 1111 1111
        */
        x |= (0xFFFF << bit_count);
    }
    return x;
}

#endif //SIGN_EXTENSION_H
