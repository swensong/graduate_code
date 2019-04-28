#ifndef _OLED_H
#define _OLED_H

#include "stm32f10x.h"
#include "spi.h"
#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#define Max_Column  128
#define Max_Row     64
#define SIZE 16 //œ‘ æ◊÷ÃÂ—°‘Ò


#define RCC_APB2Periph_OLED_PORT    RCC_APB2Periph_GPIOA
#define OLED_PORT                   GPIOA

#define OLED_CS_PIN     GPIO_Pin_2
#define OLED_CS_LOW     GPIO_ResetBits(GPIOA,GPIO_Pin_2)                        
#define OLED_CS_HIGH    GPIO_SetBits(GPIOA,GPIO_Pin_2)

#define OLED_DC_PIN     GPIO_Pin_3
#define OLED_DC_LOW     GPIO_ResetBits(GPIOA,GPIO_Pin_3)
#define OLED_DC_HIGH    GPIO_SetBits(GPIOA,GPIO_Pin_3)

#define OLED_RST_PIN    GPIO_Pin_4
#define OLED_RST_LOW    GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define OLED_RST_HIGH   GPIO_SetBits(GPIOA,GPIO_Pin_4)

#define OLED_CLK_PIN    GPIO_Pin_5
#define OLED_DATA_PIN   GPIO_Pin_7

void SPI_OLED_Init(void);
void OLED_Write_Byte(uint8_t dat);
void OLED_Write_Data(uint8_t dat);
void OLED_Write_Cmd(uint8_t cmd);


void OLED_Set_Pos(u8 x,u8 y);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Display_Clear(void);
void OLED_Display_Onechar(u8 x,u8 y,u8 str);
void OLED_Display_String(u8 x,u8 y,u8 *str);
void OLED_Display_Chinese(u8 x,u8 y,u8 no);
void OLED_Display_Picture(u8 x0,u8 y0,u8 x1,u8 y1,u8 BMP[]);
u32 OLED_Pow(u8 m,u8 n);
void OLED_Display_Num(u8 x,u8 y,u32 num,u8 len,u8 size_num);
void OLED_Init(void);

#endif


