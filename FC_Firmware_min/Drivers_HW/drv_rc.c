#include "drv_rc.h"
#include "usart.h"
#include "main.h"
#include "cmsis_os.h"
#include <string.h>


//Buffer DMA nhan
#define DMA_BUF_SIZE 32
static uint8_t dma_buf[DMA_BUF_SIZE];


//Data public
RC_Channel_t rc_data = {0};

//thoi diem nhan packet cuoi
static uint32_t last_packet_time = 0;

void DRV_RC_Init(void){
	//khoi dong DMA nhan lien tuc tren UART1
	HAL_UART_Receive_DMA(&huart1, dma_buf, DMA_BUF_SIZE);
}


void DRV_RC_ParseData(void){
	//tim header 0xAA trong buffer
	for(int i = 0; i < DMA_BUF_SIZE - RC_PACKET_SIZE + 1; i++){
		if(dma_buf[i] != RC_HEADER) continue;

		//kiem tra checksum
		uint8_t calc_cs = 0;
		for(int j = 0; j < RC_PACKET_SIZE - 1; j++){
			calc_cs ^= dma_buf[i + j];
		}
		if(calc_cs != dma_buf[i + RC_PACKET_SIZE - 1]) continue;

		//Parse data
		uint8_t *p = &dma_buf[i + 1];
		rc_data.throttle = (uint16_t)(p[0] << 8 | p[1]);
		rc_data.roll 	 = (uint16_t)(p[2] << 8 | p[3]);
		rc_data.pitch	 = (uint16_t)(p[4] << 8 | p[5]);
		rc_data.yaw		 = (uint16_t)(p[6] << 8 | p[7]);



		// Validate range 1000-2000
		if(rc_data.throttle < 1000 || rc_data.throttle > 2000) continue;
		if(rc_data.roll 	< 1000 || rc_data.roll	   > 2000) continue;
		if(rc_data.pitch	< 1000 || rc_data.pitch	   > 2000) continue;
		if(rc_data.yaw		< 1000 || rc_data.yaw	   > 2000) continue;

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
        return 0;
    }
    return 1;
}
