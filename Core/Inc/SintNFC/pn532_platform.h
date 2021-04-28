/*
 * pn532_platform.h
 *
 *  Created on: Apr 27, 2021
 *      Author: fil
 */

#ifndef INC_SINTMATE_PN532_PLATFORM_H_
#define INC_SINTMATE_PN532_PLATFORM_H_

#include "pn532.h"
//#define	ENABLE_LOG	1
void PN532_Init(PN532* dev);
int PN532_Reset(void);
void PN532_Log(const char* log);

int PN532_SPI_ReadData(uint8_t* data, uint16_t count);
int PN532_SPI_WriteData(uint8_t *data, uint16_t count);
bool PN532_SPI_WaitReady(uint32_t timeout);
int PN532_SPI_Wakeup(void);
void PN532_SPI_Init(PN532* dev);

int PN532_I2C_ReadData(uint8_t* data, uint16_t count);
int PN532_I2C_WriteData(uint8_t *data, uint16_t count);
bool PN532_I2C_WaitReady(uint32_t timeout);
int PN532_I2C_Wakeup(void);
void PN532_I2C_Init(PN532* dev);

#endif /* INC_SINTMATE_PN532_PLATFORM_H_ */
