


#include "Clock.h"


void SetClock(void)
{
    RCC_DeInit();//??????????
    RCC_HSEConfig(RCC_HSE_ON);//??????
    while(RCC_WaitForHSEStartUp()!=SUCCESS);//Waits for HSE start-up
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);//??PLL?????9? 72Mhz
		RCC_PLLCmd(ENABLE); //??PLL??                                  
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
	
		
	
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); //??????PLL?????  
	// *           - 0x00: HSI used as system clock  
	// *           - 0x04: HSE used as system clock    
	// *           - 0x08: PLL used as system clock  
	// *           - 0x0C: HSI48 used as system clock, applicable only for STM32F072 devices    
	while(RCC_GetSYSCLKSource()!=0x08); //???????????PLL???
	RCC_HCLKConfig(RCC_SYSCLK_Div1); //PLL????????   72MHz


}

