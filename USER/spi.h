#ifndef _SPI_H
#define _SPI_H

#include "stm32f10x.h"

void SPI1_Init(void);
void SPI1_SetSpeed(u8 SpeedSet);
u8 SPI1_ReadWriteByte(u8 TxData);

#endif
