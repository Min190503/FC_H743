#include "pid.h"

void PID_Init(PID_t *pid, float Kp, float Ki, float Kd, float integral_max, float output_max, float alpha){
	pid->Kp = Kp;
	pid->Ki = Ki;
	pid->Kd = Kd;

	pid->integral_max = integral_max;
	pid->output_max	  = output_max;
	pid->d_alpha	  = alpha;

	PID_Reset(pid);
}

void PID_Reset(PID_t *pid){
	pid->integral 	= 0.0f;
	pid->prev_measurement	= 0.0f;
	pid->output		= 0.0f;
	pid->d_filtered = 0.0f;
}


float PID_Compute(PID_t *pid, float setpoint, float measurement, float dt){
	//Tinh sai so
	float error = setpoint - measurement;

	//P - Proportional
	float P_out = pid->Kp * error;

	//I - Integral
	pid->integral += error * dt;

	//Anti-windup kep gia tri tich phan de khong bi cong don den vo cuc
	if(pid->integral > pid->integral_max){
		pid->integral = pid->integral_max;
	} else if(pid->integral < -pid->integral_max){
		pid->integral = -pid->integral_max;
	}
	float I_out = pid->Ki * pid->integral;

	//D - Derivative tinh tren measurement
	float derivative = -(measurement - pid->prev_measurement) / dt;
	pid->prev_measurement = measurement;

	//PT1 Low-pass filter
	pid->d_filtered = (pid->d_alpha * derivative) + ((1.0f - pid->d_alpha) * pid->d_filtered);
	float D_out = pid->Kd * pid->d_filtered;


	pid->output = P_out + I_out + D_out;
	if(pid->output > pid->output_max){
		pid->output = pid->output_max;
	} else if (pid->output < -pid->output_max){
		pid->output = -pid->output_max;
	}

	return pid->output;
}
