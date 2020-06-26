#ifndef C99BITS_H
#define C99BITS_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define CHAR_BITS  8  // size of character

#define INT_BITS  ( sizeof(int) * CHAR_BITS) //bits in integer

#define SHORT_BITS  ( sizeof(short) * CHAR_BITS) //bits in integer

// Use to print the data in binary format
void PrintInBinary(unsigned n)
{
    short int iPos;

    for (iPos = (INT_BITS -1) ; iPos >= 0 ; iPos--)
    {
        (n & (1 << iPos))? printf("1"): printf("0");
        if(iPos%4==0)
        {
            printf(" ");
        }
    }
    printf("\n");
}


void PrintInBinaryShort(unsigned short n)
{
    short int iPos;

    for (iPos = (SHORT_BITS -1) ; iPos >= 0 ; iPos--)
    {
        (n & (1 << iPos))? printf("1"): printf("0");
        if(iPos%4==0)
        {
            printf(" ");
        }
    }
    printf("\n");
}




unsigned short reverse_bit(unsigned short x)
{
    x = (x & 0xFF) << 8 | (x & 0xFF00) >>  8;
    return x;
}


unsigned int ReverseTheBits(unsigned int num)
{
    unsigned int count = (INT_BITS -1);
    unsigned int tmp = num;         //  Assign num to the tmp

    num >>= 1; // shift num because LSB already assigned to tmp

    while(num)
    {
        tmp <<= 1;  //shift the tmp because alread have the LSB of num

        tmp |= num & 1; // putting the set bits of num

        num >>= 1;

        count--;
    }

    tmp <<= count; //when num become zero shift tmp from the remaining counts

    return tmp;
}



uint16_t leftbit(  uint16_t value,   uint16_t position) {
    return (value   >> position);
}


uint16_t rightbit(  uint16_t value,   uint16_t position)
{
    return (value   << position)  ;
}








#ifdef __cplusplus
}
#endif



#endif
