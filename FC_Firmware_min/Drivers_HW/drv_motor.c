#include "drv_motor.h"
#include "cmsis_os.h"
#include <string.h>

__attribute__((section(".RAM_D2")))
static uint32_t dshot_buf_m1[DSHOT_BUF_LEN];

__attribute__((section(".RAM_D2")))
static uint32_t dshot_buf_m2[DSHOT_BUF_LEN];

__attribute__((section(".RAM_D2")))
static uint32_t dshot_buf_m3[DSHOT_BUF_LEN];

__attribute__((section(".RAM_D2")))
static uint32_t dshot_buf_m4[DSHOT_BUF_LEN];

static void encode_dshot(uint32_t *buf, uint16_t value, uint8_t telemetry){
	uint16_t packet = (value << 1) | (telemetry & 0x01);

	//tinh CRC: XOR cua 3 nibble dau
	uint16_t crc = (packet ^ (packet >> 4) ^ (packet >> 8)) & 0x0F;
	packet = (packet << 4) | crc;

	//Encode tung bit MSB truoc
	for(int i = 0; i < DSHOT_FRAME_LEN; i++){
		if(packet & (1 << (15 - i))){
			buf[i] = DSHOT_T1H; 	// bit 1
		} else {
			buf[i] = DSHOT_T0H; 	// bit 0
		}
	}
	buf[DSHOT_FRAME_LEN] = 0;		// reset pulse (LOW sau frame)
}

static void send_dshot_dma(TIM_HandleTypeDef *htim, uint32_t channel, uint32_t *buf){
	HAL_TIM_PWM_Stop_DMA(htim, channel);
	HAL_TIM_PWM_Start_DMA(htim, channel, buf, DSHOT_BUF_LEN);
}


void Motor_Init(void){
	__HAL_TIM_MOE_ENABLE(&htim8);

	for(int i = 0; i < 1000; i++){
		Motor_Stop();
		osDelay(1);
	}
}



void Motor_SetDShot(Motor_ID_t motor, uint16_t value){
	//Clamp
	if(value > DSHOT_MAX_THROTTLE) value = DSHOT_MAX_THROTTLE;

	switch(motor){
	case MOTOR_1:
		encode_dshot(dshot_buf_m1, value, 0);
		send_dshot_dma(&htim3, TIM_CHANNEL_3, dshot_buf_m1);
		break;
	case MOTOR_2:
		encode_dshot(dshot_buf_m2, value, 0);
		send_dshot_dma(&htim3, TIM_CHANNEL_4, dshot_buf_m2);
		break;
	case MOTOR_3:
		encode_dshot(dshot_buf_m3, value, 0);
		send_dshot_dma(&htim8, TIM_CHANNEL_3, dshot_buf_m3);
		break;
	case MOTOR_4:
		encode_dshot(dshot_buf_m4, value, 0);
		send_dshot_dma(&htim8, TIM_CHANNEL_4, dshot_buf_m4);
		break;
	}
}


void Motor_SetAll(uint16_t value){
	Motor_SetDShot(MOTOR_1, value);
	Motor_SetDShot(MOTOR_2, value);
	Motor_SetDShot(MOTOR_3, value);
	Motor_SetDShot(MOTOR_4, value);
}

void Motor_Stop(void){
	Motor_SetAll(DSHOT_DISARM_VALUE);
}
