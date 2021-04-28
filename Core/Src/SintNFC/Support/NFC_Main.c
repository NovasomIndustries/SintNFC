/*
 * NFC_Main.c
 *
 *  Created on: Apr 28, 2021
 *      Author: fil
 */

#include "main.h"

PN532 pn532;
uint8_t buff[256];
uint8_t uid[MIFARE_UID_MAX_LENGTH];
uint8_t key_a[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint32_t pn532_error = PN532_ERROR_NONE;
int32_t uid_len = 0;

uint8_t buf_sect[NFC_NUMBUFS][NFC_SIZEBUFS];

uint32_t NFC_Init(void)
{
	logUsartInit(&huart2);
	logUsart("Initialization Started\r\n");
	PN532_SPI_Init(&pn532);
	PN532_Reset();
	if (PN532_GetFirmwareVersion(&pn532, buff) == PN532_STATUS_OK)
		logUsart("Found PN532 with firmware version: %d.%d\r\n", buff[1], buff[2]);
	else
		return 1;
	PN532_SamConfiguration(&pn532);
	logUsart("Waiting for RFID/NFC card...\r\n");
	HAL_TIM_Base_Start_IT(&htim6);
	return 0;
}

uint32_t NFC_GetCard(void)
{
	uid_len = PN532_ReadPassiveTarget(&pn532, uid, PN532_MIFARE_ISO14443A, 1000);
	if (uid_len != PN532_STATUS_ERROR)
	{
		logUsart("Found card with UID: ");
		for (uint8_t i = 0; i < uid_len; i++)
			logUsart("%02x ", uid[i]);
		logUsart("\r\n");
		return 0;
	}
	logUsart(".");
	return 1;
}

uint32_t NFC_Read(uint32_t block_number)
{
	logUsart("Reading blocks...\r\n");
	pn532_error = PN532_MifareClassicAuthenticateBlock(&pn532, uid, uid_len,block_number, MIFARE_CMD_AUTH_A, key_a);
	if (pn532_error != PN532_ERROR_NONE)
		return 1;
	pn532_error = PN532_MifareClassicReadBlock(&pn532, buff, block_number);
	if (pn532_error != PN532_ERROR_NONE)
		return 1;
	logUsart("%d: ", block_number);
	for (uint8_t i = 0; i < 16; i++)
	{
		logUsart("%02x ", buff[i]);
	}
	logUsart("\r\n");
	return 0;
}

uint32_t NFC_SilentRead(uint32_t block_number)
{
	if ( PN532_MifareClassicAuthenticateBlock(&pn532, uid, uid_len,block_number, MIFARE_CMD_AUTH_A, key_a) == PN532_ERROR_NONE)
		return PN532_MifareClassicReadBlock(&pn532, buff, block_number);
	return 0x20;
}

uint32_t NFC_SilentRead_buff(uint32_t block_number,uint8_t *buffer)
{
	if ( PN532_MifareClassicAuthenticateBlock(&pn532, uid, uid_len,block_number, MIFARE_CMD_AUTH_A, key_a) == PN532_ERROR_NONE)
		return PN532_MifareClassicReadBlock(&pn532, buffer, block_number);
	return 0x20;
}

void NFC_InitializeBufs(void)
{
uint32_t random_number,i,j;

	for(i=NFC_1STBUFBLK;i<NFC_3RDBUFBLK+1;i++)
	{
		for(j=0;j<4;j++)
		{
			HAL_RNG_GenerateRandomNumber(&hrng, &random_number);
			buf_sect[i][j*4] 		= (random_number>>24) & 0xff;
			buf_sect[i][(j*4)+1] 	= (random_number>>16) & 0xff;
			buf_sect[i][(j*4)+2] 	= (random_number>>8)  & 0xff;
			buf_sect[i][(j*4)+3] 	=  random_number      & 0xff;
		}
	}
}

uint32_t NFC_Write(uint32_t block_number,uint8_t *buf)
{
	logUsart("Writing block %d...\r\n",block_number);
	pn532_error = PN532_MifareClassicAuthenticateBlock(&pn532, uid, uid_len,block_number, MIFARE_CMD_AUTH_A, key_a);
	if (pn532_error != PN532_ERROR_NONE)
		return 1;
	pn532_error = PN532_MifareClassicWriteBlock(&pn532, buf, block_number);
	if (pn532_error != PN532_ERROR_NONE)
		return 1;
	return 0;
}

