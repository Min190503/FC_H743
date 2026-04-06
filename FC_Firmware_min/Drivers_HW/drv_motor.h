#ifndef DRV_MOTOR_H_
#define DRV_MOTOR_H_

#include "main.h"
#include "tim.h"
#include "dma.h"

//#define MOTOR_PWM_MIN	1000
//#define MOTOR_PWM_MAX	2000
//#define MOTOR_PWM_STOP	1000

#define DSHOT_T1H		533			// 2/3 * 800 = bit 1
#define DSHOT_T0H		267			// 1/3 * 800 = bit 0
#define DSHOT_FRAME_LEN 16			// 16 bits per frame
#define DSHOT_BUF_LEN 	17			// 16 bits + 1 reset (0)

//Throttle range
#define DSHOT_MIN_THROTTLE	48		//minimum armed value
#define DSHOT_MAX_THROTTLE  2047	//maximum value
#define DSHOT_DISARM_VALUE	0		//disarm


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

void Motor_SetNormalized(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4);

#endif
