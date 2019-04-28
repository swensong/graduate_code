

//头文件
#include "stm32f10x.h"
#include "GPIOLIKE51.h"
#include"stm32f10x.h"
#include"stdarg.h"
#include"stm32f10x_usart.h"

#include	"oled.h"

#include	"delay.h"

#include "Clock.h"

#include "TiMbase.h"


#include "motor.h"

#include "Interrupt.h"


#define code  const 

#define uchar unsigned char
#define uint unsigned int
#define ulong unsigned long

//函数声明
void GPIO_Configuration(void);

//=============================================================================
//文件名称：Delay
//功能概要：延时
//参数说明：nCount：延时长短
//函数返回：无
//=============================================================================

void Delay(uint32_t nCount)
{
	for (; nCount != 0; nCount--);
}

void USART1_Config();
void Dgprintf(uint8_t *Data, ...);



uchar LCDBuff[24 * 4];



int   precalculate(char *exp);
double calculate();


char   expr[128];
int    exprlen = 0;

void LCD_printf(char *str);

char string[64];
void ftoa(float n, char *res, int afterpoint);
void dtoa(double n, char *res, int afterpoint);


int main(void)
{


	RCC_ClocksTypeDef rcc_clocks;






	GPIO_Configuration();
	USART1_Config();




	RCC_GetClocksFreq(&rcc_clocks);
	Dgprintf("\r\nSYSCLK_Frequency = %d MHz\n", rcc_clocks.SYSCLK_Frequency / 1000000);
	Dgprintf("\r\nHCLK_Frequency = %d MHz\n", rcc_clocks.HCLK_Frequency / 1000000);
	Dgprintf("\r\nPCLK1_Frequency = %d MHz\n", rcc_clocks.PCLK1_Frequency / 1000000);
	Dgprintf("\r\nPCLK2_Frequency = %d MHz\n", rcc_clocks.PCLK2_Frequency / 1000000);

	delay_init();
	Dgprintf("delay_init success!\r\n");

	OLED_Init();
	
	OLED_Display_String(0,0,"Init success!");
	Dgprintf("OLED_Init success!\r\n");
	
			
	while (1)
	{
	}

}


// reverses a string 'str' of length 'len' 
void reverse(char *str, int len)
{
	int i = 0, j = len - 1, temp;
	while (i < j)
	{
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		i++; j--;
	}
}


// Converts a given integer x to string str[].  d is the number 
// of digits required in output. If d is more than the number 
// of digits in x, then 0s are added at the beginning. 
int intToStr(int x, char str[], int d)
{
	int i = 0;

	if (x < 0) {
		//OverFlow =1;
		return -1;
	}

	if (x == 0) {
		str[0] = '0';
		str[1] = '\0';
		return 1;
	}

	while (x)
	{
		str[i++] = (x % 10) + '0';
		x = x / 10;
	}

	// If number of digits required is more, then 
	// add 0s at the beginning 
	while (i < d)
		str[i++] = '0';

	reverse(str, i);
	str[i] = '\0';
	return i;
}


int longlongToStr(long long x, char str[], int d)
{
	int i = 0;

	if (x < 0) {
		//OverFlow =1;
		return -1;
	}

	if (x == 0) {
		str[0] = '0';
		str[1] = '\0';
		return 1;
	}

	while (x)
	{
		str[i++] = (x % 10) + '0';
		x = x / 10;
	}

	// If number of digits required is more, then 
	// add 0s at the beginning 
	while (i < d)
		str[i++] = '0';

	reverse(str, i);
	str[i] = '\0';
	return i;
}





// Converts a floating point number to string. 
void ftoa(float n, char *res, int afterpoint)
{

	int offset = 0;
	int i, j;

	// Extract integer part 
	int ipart;
	float fpart;




	if (n < 0) {
		n = -n;
		res[offset] = '-';
		offset += 1;
	}



	n = n + 0.0000001;  //上溢
	ipart = (int)n;






	// Extract floating part 
	fpart = n - (float)ipart;

	// convert integer part to string 
	i = longlongToStr(ipart, res + offset, 0);
	offset += i;







	// check for display option after point 
	if (afterpoint != 0 && fpart > 0.000001)
	{

		double t0;
		long long t1;


		res[offset] = '.';  // add dot 
		offset += 1;

		// Get the value of fraction part upto given no. 
		// of points after dot. The third parameter is needed 
		// to handle cases like 233.007 

		t0 = fpart;
		for (j = 0; j < afterpoint; j++) {
			t0 = t0 * 10;
		}

		t1 = (int)t0;


		for (j = 0; j < afterpoint; j++) {
			if ((t1 / 10) * 10 != t1)
				break;

			t1 = t1 / 10;
		}

		/*
		for(j=0;j<afterpoint;j++){
		int  a10;
		a10 = (int )(fpart*10);
		if(a10 - (a10%10)*10 ==0)
		break;
		fpart = fpart *10;
		}
		*/

		longlongToStr(t1, res + offset, afterpoint - j);
	}
}




// Converts a floating point number to string. 
void dtoa(double n, char *res, int afterpoint)
{

	int offset = 0;
	int i, j;

	// Extract integer part 
	long long ipart;
	double fpart;




	if (n < 0) {
		n = -n;
		res[offset] = '-';
		offset += 1;
	}



	n = n + 0.0000001;  //上溢
	ipart = (long long)n;






	// Extract floating part 
	fpart = n - (double)ipart;

	// convert integer part to string 


	i = longlongToStr(ipart, res + offset, 0);
	offset += i;







	// check for display option after point 
	if (afterpoint != 0 && fpart > 0.000001)
	{

		double t0;
		long long  t1;


		res[offset] = '.';  // add dot 
		offset += 1;

		// Get the value of fraction part upto given no. 
		// of points after dot. The third parameter is needed 
		// to handle cases like 233.007 

		t0 = fpart;
		for (j = 0; j < afterpoint; j++) {
			t0 = t0 * 10;
		}

		t1 = (long long)t0;


		for (j = 0; j < afterpoint; j++) {
			if ((t1 / 10) * 10 != t1)
				break;

			t1 = t1 / 10;
		}

		/*
		for(j=0;j<afterpoint;j++){
		int  a10;
		a10 = (int )(fpart*10);
		if(a10 - (a10%10)*10 ==0)
		break;
		fpart = fpart *10;
		}
		*/

		longlongToStr(t1, res + offset, afterpoint - j);
	}
}





int x = 0;
int y = 0;


void LCD_printf(char *str)
{

	int i, j;

	char ch;

	for (j = 0; j < strlen(str); j++) {

		ch = str[j];

		//if(str[j] == '\\'){
		//		if(str[j+1] =='n'){						
		//			j++;					
		//			ch = 0x13;
		//		}		
		//}


		if (y >= 8) {
			//clear_screen();	

			for (i = 0; i < 3; i++)
				memcpy(LCDBuff + i * 24, LCDBuff + i * 24 + 24, 24);

			memset(LCDBuff + 3 * 24, 0, 24);


			{

				int xx, yy;

				for (yy = 0; yy < 6; yy += 2) {
					for (xx = 0; xx < 24; xx++) {
						uchar  a = LCDBuff[xx + yy / 2 * 24];
						//	if(a!=0)						
						//display_char_8x16(yy ,xx*8,a);						
					}
				}
			}

			y = 6;
		}

		if (ch == '\n') {
			x = 0;
			y += 2;
		}
		else {

			//display_char_8x16(y ,x,(uchar)(ch) );



			LCDBuff[y / 2 * 24 + x / 8] = ch;

			x += 8;
			if (x >= 192) {
				x = 0;
				y += 2;
			}
		}

	}

	return;
}

void LCD_backspace()
{
	if (x > 0)
		x -= 8;
	else
		x = 192 - 8;

	//	display_char_8x16(y ,x,(uchar)(' ') );


}


/*



//OUT
#define  SW_R0      //PC14
#define  SW_R1      //PC13
#define  SW_R2      //PB9
#define  SW_R3      //PB8
#define  SW_R4      //PB7



//IN
#define  SW_C0      //PB6
#define  SW_C1      //PB5
#define  SW_C2      //PB4
#define  SW_C3      //PB3
#define  SW_C4      //PA15



*/


//=============================================================================
//文件名称：GPIO_Configuration
//功能概要：GPIO初始化
//参数说明：无
//函数返回：无
//=============================================================================
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

















	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);




	//=============================================================================
	//PB2
	//=============================================================================			 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);




	//=============================================================================
	//PB3
	//=============================================================================			 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	
	//=============================================================================
	//PB4
	//=============================================================================			 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	
	//=============================================================================
	//PB5
	//=============================================================================			 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	
	

	//=============================================================================
	//PB6
	//=============================================================================			 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);



	//=============================================================================
	//PB7
	//=============================================================================			 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);



	//=============================================================================
	//PB8
	//=============================================================================			 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);




	//=============================================================================
	//PB9
	//=============================================================================			 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	//=============================================================================
	//PB10
	//=============================================================================			 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	//=============================================================================
	//PB11
	//=============================================================================			 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);





	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
	

	//=============================================================================
	//PD2
	//=============================================================================			 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
		



	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

}




void USART1_Config()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* ??USART1??? */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

	/*??USART1 TX,????????????*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/*??USART1 RX,??????????*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);



	USART_InitStructure.USART_BaudRate = 115200;                               //??????57600//
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;               //?????????8?//
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                    //??????1//
	USART_InitStructure.USART_Parity = USART_Parity_No;                       //????????//
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //??????//
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//????????????,?RX?Tx?????//
	USART_Init(USART1, &USART_InitStructure);																	//??????????//
	USART_Cmd(USART1, ENABLE);																								//??USART1??// 








}



static char *itoa(int value, char *string, int radix)
{
	int i, d;
	int flag = 0;
	char *ptr = string;

	/*???????????*/
	if (radix != 10)
	{
		*ptr = 0;
		return string;
	}

	if (!value)
	{
		*ptr++ = 0x30;
		*ptr = 0;
		return string;
	}

	/*??????,?????*/
	if (value < 0)
	{
		*ptr++ = '-';
		value *= -1;
	}
	for (i = 10000; i > 0; i /= 10)
	{
		d = value / i;
		if (d || flag)
		{
			*ptr++ = (char)(d + 0x30);
			value -= (d * i);
			flag = 1;
		}
	}

	/*NULL?????*/
	*ptr = 0;
	return string;
}




void Dgprintf(uint8_t *Data, ...)
{
	const char *s;
	int d;
	char buf[16];
	USART_TypeDef* USARTx;
	char  c;





	va_list ap;

	USARTx = USART1;


	va_start(ap, Data);
	while (*Data != 0)                                    //????????????//
	{
		if (*Data == 0x5c)                                   //'\'//
		{
			switch (*++Data)
			{
			case 'r':                                       //???//
				USART_SendData(USARTx, 0x0d);
				Data++;
				break;

			case 'n':
				USART_SendData(USARTx, 0x0d);					//???//
				USART_SendData(USARTx, 0x0a);
				Data++;
				break;

			default:
				Data++;
				break;
			}
		}
		else if (*Data == '%')
		{
			switch (*++Data)
			{
			case 's':                                        //???//
				s = va_arg(ap, const char *);
				for (; *s; s++)
				{
					USART_SendData(USARTx, *s);
					while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
				}
				Data++;
				break;

			case 'd':                                       //???//
				d = va_arg(ap, int);
				itoa(d, buf, 10);
				for (s = buf; *s; s++)
				{
					USART_SendData(USARTx, *s);
					while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
				}
				Data++;
				break;


			case 'c':
				c = va_arg(ap, const char);
				USART_SendData(USARTx, c);
				while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
				Data++;

				break;
			default:
				Data++;
				break;
			}
		}
		else USART_SendData(USARTx, *Data++);
		while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
	}
}


