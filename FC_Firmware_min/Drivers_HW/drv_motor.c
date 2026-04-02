#include "drv_motor.h"
#include "cmsis_os.h"

void Motor_Init(void){
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_4);

	__HAL_TIM_MOE_ENABLE(&htim8);

	Motor_Stop();
	osDelay(3000);
}


void Motor_SetPWM(Motor_ID_t motor, uint16_t pwm_us){
	if(pwm_us < MOTOR_PWM_MIN) pwm_us = MOTOR_PWM_MIN;
	if(pwm_us > MOTOR_PWM_MAX) pwm_us = MOTOR_PWM_MAX;


	switch(motor){
	case MOTOR_1:
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, pwm_us);
		break;
	case MOTOR_2:
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, pwm_us);
		break;
	case MOTOR_3:
		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_3, pwm_us);
		break;
	case MOTOR_4:
		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_4, pwm_us);
		break;
	}
}


void Motor_SetAll(uint16_t pwm_us){
	Motor_SetPWM(MOTOR_1, pwm_us);
	Motor_SetPWM(MOTOR_2, pwm_us);
	Motor_SetPWM(MOTOR_3, pwm_us);
	Motor_SetPWM(MOTOR_4, pwm_us);
}

void Motor_Stop(void){
	Motor_SetAll(MOTOR_PWM_STOP);
}
