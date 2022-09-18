#ifndef UPDATE_CONDITION_FLAGS_H
#define UPDATE_CONDITION_FLAGS_H

#include "../registers.h"

/* Whenever a value is written to a register, we need to update the condition flag to indicate its sign.  */
void update_condition_flags(uint16_t r)
{
    if(registers[r] == 0)
    {
        registers[R_COND] = FL_ZER;
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

#endif //UPDATE_CONDITION_FLAGS_H
