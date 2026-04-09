#ifndef MIXER_H
#define MIXER_H

#include <stdint.h>
#include "pid.h"

//dau ra motor
typedef struct {
	uint16_t m1;
	uint16_t m2;
	uint16_t m3;
	uint16_t m4;
}MotorOutput_t;
extern PID_t pid_roll;
extern PID_t pid_pitch;
extern PID_t pid_yaw;
extern PID_t pid_angle_roll;
extern PID_t pid_angle_pitch;

void Mixer_Init(void);
MotorOutput_t Mixer_Compute(uint16_t throttle_rc,
							float roll_angle, float pitch_angle,
							float gyro_roll, float gyro_pitch, float gyro_yaw,
							float roll_target, float pitch_target, float yaw_target,
							float dt);
#endif
