#ifdef _SYSTEM_IS_BIG_ENDIAN
#include "endian.h"

short swap_short_endian(short value)
{
    return (short)(((unsigned short)(value) << 8) | ((unsigned short)(value) >> 8));
}

void swap_endian(short vec[], long len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        vec[i] = swap_short_endian(vec[i]);
    }
}
#endif