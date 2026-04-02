#include "mixer.h"

PID_t pid_roll;
PID_t pid_pitch;
PID_t pid_yaw;

static float constrain_float(float val, float min, float max){
	if(val < min) return min;
	if(val > max) return max;
	return val;
}


void Mixer_Init(void){

	//Roll & Pitch (Điều khiển Góc - Angle): Cần P lớn để phản ứng nhanh, D để giảm dao động
	PID_Init(&pid_roll, 2.5f, 0.0f, 0.5f, 300.0f, 400.0f);
	PID_Init(&pid_pitch, 2.5f, 0.0f, 0.5f, 300.0f, 400.0f);


	//Yaw (Điều khiển Tốc độ xoay - Rate): Thường cần P lớn hơn, I để giữ đuôi không bị trôi
	PID_Init(&pid_yaw, 4.0f, 1.0f, 0.0f, 200.0f, 400.0f);
}

MotorOutput_t Mixer_Compute(uint16_t throttle_rc,
							float roll_angle, float pitch_angle, float yaw_rate,
							float roll_target, float pitch_target, float yaw_target,
							float dt){
	MotorOutput_t out = {0, 0, 0, 0};

	//Nếu ga dưới 5%, tắt motor và RESET bộ tích phân (I) để rác không bị cộng dồn
	if(throttle_rc < 50){
		PID_Reset(&pid_roll);
		PID_Reset(&pid_pitch);
		PID_Reset(&pid_yaw);
		return out;
	}

	//Tinh PID
	float out_roll = PID_Compute(&pid_roll, roll_target, roll_angle, dt);
	float out_pitch = PID_Compute(&pid_pitch, pitch_target, pitch_angle, dt);
	float out_yaw = PID_Compute(&pid_yaw, yaw_target, yaw_rate, dt);


	//M1 sau phai CW
	float m1 = throttle_rc + out_pitch - out_roll + out_yaw;

	//M2 truoc phai  CCW
	float m2 = throttle_rc - out_pitch - out_roll - out_yaw;

	//M3 sau trai CCW
	float m3 = throttle_rc + out_pitch + out_roll - out_yaw;

	//M4 truoc trai  CW
	float m4 = throttle_rc - out_pitch + out_roll + out_yaw;


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
