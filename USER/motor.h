



#ifndef MOTOR_H
#define MOTOR_H


#define MOTOR0_DIR_1			PBout(11)=1			//PB11
#define MOTOR0_DIR_0			PBout(11)=0			//PB11


#define MOTOR0_PULSE_1		PBout(10)=1	 //PB10
#define MOTOR0_PULSE_0		PBout(10)=0	 //PB10


#define MOTOR0_EN_1			PBout(2)=1 //PB2
#define MOTOR0_EN_0			PBout(2)=0 //PB2



















void  motor_start();
void  moto_stop();


#endif




