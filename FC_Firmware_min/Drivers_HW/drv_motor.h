#ifndef DRV_MOTOR_H_
#define ERV_MOTOR_H_

#include "main.h"
#include "tim.h"

#define MOTOR_PWM_MIN	1000
#define MOTOR_PWM_MAX	2000
#define MOTOR_PWM_STOP	1000


typedef enum {
	MOTOR_1 = 0,	//TIM3_CH3	PB0
	MOTOR_2,		//TIM3_CH4	PB1
	MOTOR_3,		//TIM8_CH3	PC8
	MOTOR_4			//TIM8_CH4	PC9
} Motor_ID_t;

void Motor_Init(void);
void Motor_SetPWM(Motor_ID_t motor, uint16_t pwm_us);
void Motor_SetAll(uint16_t pwm_us);
void Motor_Stop(void);

#endif
