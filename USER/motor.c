


#include "stm32f10x.h"
#include "GPIOLIKE51.h"
#include"stm32f10x.h"
#include"stdarg.h"


#include  "motor.h"



void  motor_start()
{
	
	
	MOTOR0_DIR_1;
	
	
	MOTOR0_EN_1;
	
	//HAL_TIM_Base_Start_IT(&htim1); 
	
}



void  motor_stop()
{
	
	
	
	
	
	MOTOR0_EN_0;	
}



