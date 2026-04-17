#include "mixer.h"

#define MOTOR_IDLE 60

PID_t pid_roll;
PID_t pid_pitch;
PID_t pid_yaw;
PID_t pid_angle_roll;
PID_t pid_angle_pitch;


static float constrain_float(float val, float min, float max){
	if(val < min) return min;
	if(val > max) return max;
	return val;
}


void Mixer_Init(void){

	// === GIÁ TRỊ AN TOÀN CHO BAY LẦN ĐẦU (BUỘC DÂY) ===
	// Kd = 0: Tắt D để tránh giật do rung cánh quạt
	// Kp thấp: Phản ứng chậm nhưng KHÔNG LẬT
	// Ki = 0: Không tích phân, tránh windup

	//Angle Loop - dung P
	PID_Init(&pid_angle_roll,  5.0f, 0.0f, 0.0f, 0.0f, 300.0f, 0.0f);
	PID_Init(&pid_angle_pitch, 5.0f, 0.0f, 0.0f, 0.0f, 300.0f, 0.0f);

	//Rate Loop _ P+I+D
	PID_Init(&pid_roll,  2.0f, 0.30f, 0.005f, 100.0f, 400.0f, 0.25f);
	PID_Init(&pid_pitch, 2.0f, 0.30f, 0.005f, 100.0f, 400.0f, 0.25f);
	PID_Init(&pid_yaw,   3.0f, 0.4f,  0.0f,   100.0f, 400.0f, 0.0f);
}

MotorOutput_t Mixer_Compute(uint16_t throttle_rc,
							float roll_angle, float pitch_angle,
							float gyro_roll, float gyro_pitch, float gyro_yaw,
							float roll_target, float pitch_target, float yaw_target,
							float dt){
	MotorOutput_t out = {0, 0, 0, 0};

	//Nếu ga dưới 100, tắt motor và RESET bộ tích phân (I) để rác không bị cộng dồn
	if(throttle_rc < 100){
		PID_Reset(&pid_angle_roll);
		PID_Reset(&pid_angle_pitch);
		PID_Reset(&pid_roll);
		PID_Reset(&pid_pitch);
		PID_Reset(&pid_yaw);
		return out;
	}
	if(throttle_rc < MOTOR_IDLE){
		throttle_rc = MOTOR_IDLE;
	}

	//Angle Loop
	float rate_target_roll  = PID_Compute(&pid_angle_roll, roll_target, roll_angle, dt);
	float rate_target_pitch = PID_Compute(&pid_angle_pitch, pitch_target, pitch_angle, dt);

	//Rate Loop
	//Tinh PID
	float out_roll = PID_Compute(&pid_roll, rate_target_roll, gyro_roll, dt);
	float out_pitch = PID_Compute(&pid_pitch, rate_target_pitch, gyro_pitch, dt);
	float out_yaw = PID_Compute(&pid_yaw, yaw_target, gyro_yaw, dt);


	//M1 sau phai CW
	float m1 = throttle_rc + out_pitch - out_roll - out_yaw;

	//M2 truoc phai  CCW
	float m2 = throttle_rc - out_pitch - out_roll + out_yaw;

	//M3 sau trai CCW
	float m3 = throttle_rc + out_pitch + out_roll + out_yaw;

	//M4 truoc trai  CW
	float m4 = throttle_rc - out_pitch + out_roll - out_yaw;


	//Goi han an toan
	m1 = constrain_float(m1, 0.0f, 1000.0f);
	m2 = constrain_float(m2, 0.0f, 1000.0f);
	m3 = constrain_float(m3, 0.0f, 1000.0f);
	m4 = constrain_float(m4, 0.0f, 1000.0f);


	//dong goi xuat ra
	out.m1 = (uint16_t)m1;
	out.m2 = (uint16_t)m2;
	out.m3 = (uint16_t)m3;
	out.m4 = (uint16_t)m4;

	return out;
}
