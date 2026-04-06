#include "drv_rc.h"
#include "usart.h"
#include "main.h"
#include "cmsis_os.h"
#include <string.h>


//Buffer DMA nhan
#define DMA_BUF_SIZE 64
uint8_t dma_buf[DMA_BUF_SIZE] __attribute__((section(".RAM_D2"))) __attribute__((aligned(32)));

//Data public
RC_Channel_t rc_data = {0};

//thoi diem nhan packet cuoi
static uint32_t last_packet_time = 0;

void DRV_RC_Init(void){
	huart1.Instance->CR3 |= USART_CR3_OVRDIS;

	//khoi dong DMA nhan lien tuc tren UART1
	HAL_UART_Receive_DMA(&huart1, dma_buf, DMA_BUF_SIZE);
}


void DRV_RC_ParseData(void){
	static uint16_t last_ndtr = DMA_BUF_SIZE;
	uint16_t current_ndtr = __HAL_DMA_GET_COUNTER(huart1.hdmarx);

	if(current_ndtr == last_ndtr){
		return;
	}
	last_ndtr = current_ndtr;

	//tim header 0xAA trong buffer
	SCB_InvalidateDCache_by_Addr((uint32_t *)dma_buf, DMA_BUF_SIZE);

	for(int i = 0; i < DMA_BUF_SIZE; i++){
		if(dma_buf[i] != RC_HEADER) continue;

		//kiem tra checksum
		uint8_t calc_cs = 0;
		for(int j = 0; j < RC_PACKET_SIZE - 1; j++){
			calc_cs ^= dma_buf[(i + j) % DMA_BUF_SIZE];
		}

		if(calc_cs != dma_buf[(i + RC_PACKET_SIZE - 1) % DMA_BUF_SIZE]) continue;

		RC_Packet_t packet;
		uint8_t *p_temp = (uint8_t *)&packet;
		for(int j = 0; j < RC_PACKET_SIZE; j++ ){
			p_temp[j] = dma_buf[(i + j) % DMA_BUF_SIZE];
		}


		// Validate range 1000-2000
		if(packet.throttle 	< 1000 || packet.throttle 	> 2000) continue;
		if(packet.roll 		< 1000 || packet.roll	   	> 2000) continue;
		if(packet.pitch		< 1000 || packet.pitch	   	> 2000) continue;
		if(packet.yaw		< 1000 || packet.yaw	   	> 2000) continue;

		//packet hop le
		rc_data.throttle = packet.throttle;
		rc_data.roll     = packet.roll;
		rc_data.pitch    = packet.pitch;
		rc_data.yaw      = packet.yaw;

		// Map cong tac tu (0/1) sang chuan RC (1000/2000) de Arm/Disarm
		rc_data.aux1 = (packet.sw1 > 0) ? 2000 : 1000;
		rc_data.aux2 = (packet.sw2 > 0) ? 2000 : 1000;


		//packet hop le
		rc_data.is_failsafe = 0;
		last_packet_time = osKernelGetTickCount();

		dma_buf[i] = 0x00;
		break;
	}
}

uint8_t DRV_RC_IsHealthy(void){
    uint32_t now = osKernelGetTickCount();
    if((now - last_packet_time) > RC_FAILSAFE_TIMEOUT){
        rc_data.is_failsafe = 1;
        // Failsafe: về giá trị an toàn
        rc_data.throttle = 1000;
        rc_data.roll     = 1500;
        rc_data.pitch    = 1500;
        rc_data.yaw      = 1500;
        rc_data.aux1     = 1000;
        rc_data.aux2     = 1000;
        return 0;
    }
    return 1;
}
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart){
    if(huart->Instance == USART1){
        // Xoá toàn bộ cờ lỗi
        __HAL_UART_CLEAR_OREFLAG(huart);
        __HAL_UART_CLEAR_NEFLAG(huart);
        __HAL_UART_CLEAR_FEFLAG(huart);

        // Huỷ phiên DMA cũ đã chết
        HAL_UART_DMAStop(huart);

        // Khởi động lại DMA mới tinh
        HAL_UART_Receive_DMA(huart, dma_buf, DMA_BUF_SIZE);
    }
}
