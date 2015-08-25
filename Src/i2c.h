#ifndef __I2C_H
#define __I2C_H

#include "Beacon_board.h"
#include "stm32l1xx.h"

void I2C1_Init(uint32_t ClockSpeed);
void I2C1_DeInit(void);
bool I2C_SendByte(uint8_t Address, uint8_t Register, uint8_t Data);
bool I2C_ReceiveWord(uint8_t Address, uint8_t Register, uint16_t *Data);
bool I2C_ReceiveDWord(uint8_t Address, uint8_t Register, uint16_t *Data1, uint16_t *Data2);

#endif
