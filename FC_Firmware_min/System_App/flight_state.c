#include "flight_state.h"

volatile Flight_Status_t flight_status = {0};

void FlightState_Init(void){
	flight_status.state = STATE_INIT;
	flight_status.mode 	= MODE_ANGLE;
	flight_status.is_calibrated = 0;
}

void FlightState_Update(void){
	if(rc_data.is_failsafe == 1){
		flight_status.state = STATE_FAILSAFE;
		return;
	}

	if(rc_data.aux2 > 1500){
		flight_status.mode = MODE_ANGLE;
	} else {
		flight_status.mode = MODE_ACRO;
	}

	switch (flight_status.state){
	case STATE_INIT:
		if (flight_status.is_calibrated == 1){
			flight_status.state = STATE_DISARMED;
		}
		break;
	case STATE_DISARMED:
		if(rc_data.aux1 > 1500 && rc_data.throttle < 1010) {
			flight_status.state = STATE_ARMED;
		}
		break;
	case STATE_ARMED:
		if(rc_data.aux1 < 1500){
			flight_status.state = STATE_DISARMED;
		}
		break;
	case STATE_FAILSAFE:
		if(rc_data.is_failsafe == 0){
			flight_status.state = STATE_DISARMED;
		}
		break;
	}
}
