#include <stdio.h>
#include <stdlib.h>

#define uint8_t     unsigned char
#define uint16_t    unsigned short
#define uint32_t    unsigned int
#define UID_SIZE    4

#define SHIFT_01    7

#define TRY         2


/*
encoder logic :
add tries to uid
rotate right 7
negate
xchange

decoder logic :
xchange
negate
rotate right 7
subtract uid
*/

uint8_t uid[UID_SIZE] =
{
    0x03,
    0x54,
    0xa5,
    0x15
};

uint8_t cuid[UID_SIZE], result[UID_SIZE] , shftnum;

uint8_t ror(uint8_t value, uint8_t number_of_shifts)
{
uint8_t i;
    for(i=0;i<number_of_shifts;i++)
    {
        if (( value & 0x01 ) == 0x01)
            value = (value >> 1 ) | 0x80;
        else
            value = value >> 1;
    }
    return value;
}

uint8_t rol(uint8_t value, uint8_t number_of_shifts)
{
uint8_t i;
    for(i=0;i<number_of_shifts;i++)
    {
        if (( value & 0x80 ) == 0x80)
            value = (value << 1 ) | 0x01;
        else
            value = value << 1;
    }
    return value;
}

uint8_t neg(uint8_t value)
{
    return value ^= 0xff;
}

uint8_t xchange(uint8_t tries)
{
uint8_t i;

    if ( tries == 2 )
    {
        i = cuid[2];
        cuid[2] = cuid[0];
        cuid[0] = i;

        i = cuid[3];
        cuid[3] = cuid[1];
        cuid[1] = i;
    }
    if ( tries == 1 )
    {
        i = cuid[2];
        cuid[2] = cuid[3];
        cuid[3] = i;

        i = cuid[0];
        cuid[0] = cuid[1];
        cuid[1] = i;
    }

    return 0;
}

uint8_t check(uint8_t tries)
{
uint8_t i;
    for(i=0;i<UID_SIZE;i++)
    {
        if (( cuid[i] - uid[i] ) != tries)
            return 1;
    }
    return 0;
}

void encode(uint8_t tries)
{
uint8_t i;

    shftnum = SHIFT_01 + tries;
    printf("********** ENCODE ********************\n");
    printf("tries                         : 0x%02x\n",tries);

    printf("uid                           : 0x%02x,0x%02x,0x%02x,0x%02x\n",uid[0],uid[1],uid[2],uid[3]);
    for(i=0;i<UID_SIZE;i++)
        cuid[i] = uid[i]+tries;
    #ifdef INTERMEDIATE
    printf("cuid stage 0 , add tries      : 0x%02x,0x%02x,0x%02x,0x%02x\n",cuid[0],cuid[1],cuid[2],cuid[3]);
    #endif
    for(i=0;i<UID_SIZE;i++)
        cuid[i] = ror(cuid[i],shftnum);
    #ifdef INTERMEDIATE
    printf("cuid stage 1 , rotate right %d : 0x%02x,0x%02x,0x%02x,0x%02x\n",SHIFT_01,cuid[0],cuid[1],cuid[2],cuid[3]);
    #endif
    for(i=0;i<UID_SIZE;i++)
        cuid[i] = neg(cuid[i]);
    #ifdef INTERMEDIATE
    printf("cuid stage 2 , negate         : 0x%02x,0x%02x,0x%02x,0x%02x\n",cuid[0],cuid[1],cuid[2],cuid[3]);
    #endif
    xchange(tries);
    #ifdef INTERMEDIATE
    printf("cuid stage 2 , xchange        : 0x%02x,0x%02x,0x%02x,0x%02x\n\n",cuid[0],cuid[1],cuid[2],cuid[3]);
    #endif
    for(i=0;i<UID_SIZE;i++)
        result[i] = cuid[i];
    printf("********** ENCODE RESULT      : 0x%02x,0x%02x,0x%02x,0x%02x ****\n\n",result[0],result[1],result[2],result[3]);

}

void decode(uint8_t tries)
{
uint8_t i;

    shftnum = SHIFT_01 + tries;
    #ifdef INTERMEDIATE
    printf("********** DECODE ********************\n");
    #endif
    for(i=0;i<UID_SIZE;i++)
        cuid[i] = result[i];
    #ifdef INTERMEDIATE
    printf("start from                    : 0x%02x,0x%02x,0x%02x,0x%02x\n",cuid[0],cuid[1],cuid[2],cuid[3]);
    #endif
    xchange(tries);
    #ifdef INTERMEDIATE
    printf("cuid stage 0 , xchange        : 0x%02x,0x%02x,0x%02x,0x%02x\n",cuid[0],cuid[1],cuid[2],cuid[3]);
    #endif
    for(i=0;i<UID_SIZE;i++)
        cuid[i] = neg(cuid[i]);
    #ifdef INTERMEDIATE
    printf("cuid stage 1 , negate         : 0x%02x,0x%02x,0x%02x,0x%02x\n",cuid[0],cuid[1],cuid[2],cuid[3]);
    #endif
    for(i=0;i<UID_SIZE;i++)
        cuid[i] = rol(cuid[i],shftnum);
    #ifdef INTERMEDIATE
    printf("cuid stage 2 , rotate left %d  : 0x%02x,0x%02x,0x%02x,0x%02x\n",SHIFT_01,cuid[0],cuid[1],cuid[2],cuid[3]);
    #endif
    if ( check(tries) == 0 )
        printf("********** DECODE RESULT       : Success , tries is %d ******\n",tries);
    else
        printf("********** DECODE RESULT       : Error   ********************\n");
    printf("\n");
}

int main()
{
uint8_t i,tries=TRY;

    encode(tries);
    decode(tries);
    tries--;
    encode(tries);
    decode(tries);
    tries--;
    encode(tries);
    decode(tries);

    return 0;
}
