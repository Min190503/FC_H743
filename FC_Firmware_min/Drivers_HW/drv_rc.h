#ifndef DRV_RC_H_
#define DRV_RC_H_

#include "stdint.h"

#define RC_PACKET_SIZE  10  // header(1) + 4 kênh x2byte + checksum(1)
#define RC_HEADER       0xAA
#define RC_FAILSAFE_TIMEOUT  200

typedef struct {
	uint16_t roll;
	uint16_t pitch;
	uint16_t throttle;
	uint16_t yaw;
	uint16_t aux1;		//Cong tac Arm
	uint16_t aux2;		//cong tac mode(Acro/Angle)

	uint8_t is_failsafe;	//mat song
} RC_Channel_t;

extern RC_Channel_t rc_data;

void DRV_RC_Init(void);
void DRV_RC_ParseData(void);
uint8_t DRV_RC_IsHealthy(void);


#endif
