#ifndef FLIGHT_STATE_H
#define FLIGHT_STATE_H

#include "main.h"
#include "drv_rc.h"
#include <stdint.h>


typedef enum {
	STATE_INIT = 0,		//Dang khoi dong calib IMU
	STATE_DISARMED,		//ngat dong co
	STATE_ARMED,		//ss
	STATE_FAILSAFE		//mat song
} System_State_t;

//Flight mode
typedef enum {
	MODE_ACRO = 0,	//sport
	MODE_ANGLE		//Can bang
}Flight_Mode_t;

//tong hop trang thai
typedef struct {
	System_State_t 	state;
	Flight_Mode_t	mode;
	uint8_t			is_calibrated;
}Flight_Status_t;

extern volatile Flight_Status_t flight_status;

void FlightState_Init(void);
void FlightState_Update(void);

#endif
