/*
 * NFC_EncDec.c
 *
 *  Created on: Apr 28, 2021
 *      Author: fil
 */
#include "main.h"

#define UID_SIZE    4
#define SHIFT_01    7
#define TRY         6

/*
encoder logic :
add tries to uid
rotate right 7
negate
xchange

decoder logic :
xchange
negate
rotate left 7
subtract uid
*/

uint8_t host_buff[64];

static void NFC_MessBuffer(void)
{
uint32_t random_number,j;

	for(j=0;j<16;j++)
	{
		HAL_RNG_GenerateRandomNumber(&hrng, &random_number);
		host_buff[j*4] 		= (random_number>>24) & 0xff;
		host_buff[(j*4)+1] 	= (random_number>>16) & 0xff;
		host_buff[(j*4)+2] 	= (random_number>>8)  & 0xff;
		host_buff[(j*4)+3] 	=  random_number      & 0xff;
	}
}

#ifdef ALGO_1
static 	uint8_t cuid[UID_SIZE], result[UID_SIZE] , shftnum;

static uint8_t ror(uint8_t value, uint8_t number_of_shifts)
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

static uint8_t rol(uint8_t value, uint8_t number_of_shifts)
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

static 	uint8_t neg(uint8_t value)
{
    return value ^= 0xff;
}

static uint8_t xchange(uint8_t tries)
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

static uint8_t check(uint8_t tries)
{
uint8_t i;
    for(i=0;i<UID_SIZE;i++)
    {
        if (( cuid[i] - uid[i] ) != tries)
            return 1;
    }
    return 0;
}


static void get_cuid(uint8_t tries)
{
uint8_t i;
	if ( tries == 2)
		NFC_SilentRead(NFC_3RDBUFBLK);
	if ( tries == 1 )
    	NFC_SilentRead(NFC_2NDBUFBLK);
	for(i=0;i<UID_SIZE;i++)
		cuid[i] = buff[i];
}

static void NFC_FillHostBuffer(void)
{
	NFC_SilentRead_buff(NFC_0BUFBLK,&host_buff[0]);
	NFC_SilentRead_buff(NFC_1STBUFBLK,&host_buff[16]);
	NFC_SilentRead_buff(NFC_2NDBUFBLK,&host_buff[32]);
	NFC_SilentRead_buff(NFC_3RDBUFBLK,&host_buff[48]);
}



void encode(uint8_t tries)
{
uint8_t i;
    shftnum = SHIFT_01 + tries;
    for(i=0;i<UID_SIZE;i++)
        cuid[i] = uid[i]+tries;
    for(i=0;i<UID_SIZE;i++)
        cuid[i] = ror(cuid[i],shftnum);
    for(i=0;i<UID_SIZE;i++)
        cuid[i] = neg(cuid[i]);
    xchange(tries);
    for(i=0;i<UID_SIZE;i++)
        result[i] = cuid[i];
    NFC_InitializeBufs();
    if ( tries == 2)
    {
        for(i=0;i<UID_SIZE;i++)
        	buf_sect[NFC_3RDBUFBLK][i] = result[i];
    }
    if ( tries == 1)
    {
        for(i=0;i<UID_SIZE;i++)
        	buf_sect[NFC_2NDBUFBLK][i] = result[i];
    }
}

uint8_t decode(uint8_t tries)
{
uint8_t i;

	get_cuid(tries);
    shftnum = SHIFT_01 + tries;
    xchange(tries);
    for(i=0;i<UID_SIZE;i++)
        cuid[i] = neg(cuid[i]);
    for(i=0;i<UID_SIZE;i++)
        cuid[i] = rol(cuid[i],shftnum);
    if ( check(tries) == 0 )
    {
    	logUsart("DECODE RESULT : Success , tries is %d\r\n",tries);
    	NFC_FillHostBuffer();
    }
    else
    {
    	logUsart("DECODE RESULT : Error\r\n");
    	NFC_MessBuffer();
    }
    logUsart("\n");
}

#endif // ALGO_1
#ifdef ALGO_2
#endif // ALGO_2

void encode(uint8_t tries)
{
	NFC_InitializeBufs();
	buf_sect[NFC_3RDBUFBLK][0x0c] = uid[0]+tries;
	buf_sect[NFC_3RDBUFBLK][0x08] = uid[1]+tries;
	buf_sect[NFC_3RDBUFBLK][0x04] = uid[2]+tries;
	buf_sect[NFC_3RDBUFBLK][0x00] = uid[3]+tries;
}

uint8_t decode(uint8_t tries)
{
uint8_t buffer[16];

	NFC_SilentRead_buff(NFC_3RDBUFBLK,buffer);

	NFC_MessBuffer();
	if (( buffer[0x0c] - uid[0] ) == tries)
	{
		if (( buffer[0x08] - uid[1] ) == tries)
		{
			if (( buffer[0x04] - uid[2] ) == tries)
			{
				if (( buffer[0x00] - uid[3] ) == tries)
				{
					NFC_SilentRead_buff(NFC_0BUFBLK,&host_buff[0]);
					NFC_SilentRead_buff(NFC_1STBUFBLK,&host_buff[16]);
					NFC_SilentRead_buff(NFC_2NDBUFBLK,&host_buff[32]);
					NFC_SilentRead_buff(NFC_3RDBUFBLK,&host_buff[48]);
			    	logUsart("DECODE RESULT with tries %d : Success\r\n",tries);
			    	return 0;
				}
			}
		}
	}
	logUsart("DECODE RESULT with tries %d: Error\r\n", tries);
	return 1;
}
