#ifndef DRV_RC_H_
#define DRV_RC_H_

#include "stdint.h"

#define RC_PACKET_SIZE  12
#define RC_HEADER       0xAA
#define RC_FAILSAFE_TIMEOUT  200

typedef struct {
	uint8_t header;
	uint16_t throttle;
	uint16_t roll;
	uint16_t pitch;
	uint16_t yaw;
	uint8_t sw1;		//Cong tac Arm
	uint8_t sw2;		//cong tac mode(Acro/Angle)

	uint8_t checksum;	//mat song
}  __attribute__((packed)) RC_Packet_t;

typedef struct {
	uint16_t throttle;
	uint16_t roll;
	uint16_t pitch;
	uint16_t yaw;
	uint16_t aux1;
	uint16_t aux2;
	uint8_t is_failsafe;
} RC_Channel_t;

extern RC_Channel_t rc_data;

void DRV_RC_Init(void);
void DRV_RC_ParseData(void);
uint8_t DRV_RC_IsHealthy(void);


#endif
