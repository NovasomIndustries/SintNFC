/*
 * NFC_Main.h
 *
 *  Created on: Apr 28, 2021
 *      Author: fil
 */

#ifndef INC_SINTNFC_NFC_MAIN_H_
#define INC_SINTNFC_NFC_MAIN_H_

#define	NFC_NUMBUFS		3
#define	NFC_SIZEBUFS	16
#define	NFC_0BUFBLK		0
#define	NFC_1STBUFBLK	4
#define	NFC_2NDBUFBLK	5
#define	NFC_3RDBUFBLK	6

extern	uint8_t buf_sect[NFC_NUMBUFS][NFC_SIZEBUFS];
extern	uint8_t uid[MIFARE_UID_MAX_LENGTH];
extern	uint8_t buff[256];

extern	uint32_t NFC_Init(void);
extern	uint32_t NFC_GetCard(void);
extern	uint32_t NFC_Read(uint32_t block_number);
extern	uint32_t NFC_Write(uint32_t block_number,uint8_t *buf);
extern	void NFC_InitializeBufs(void);
extern	uint32_t NFC_SilentRead(uint32_t block_number);
extern	uint32_t NFC_SilentRead_buff(uint32_t block_number,uint8_t *buffer);

#endif /* INC_SINTNFC_NFC_MAIN_H_ */
