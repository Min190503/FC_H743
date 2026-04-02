#ifndef PID_H
#define PID_H

typedef struct {
	float Kp;
	float Ki;
	float Kd;

	float integral;		//tich luy sai so (I)
	float prev_error;	// sai so lan truoc (D)
	float output;		//ket qua dau ra


	//Gioi han
	float integral_max; //chong tran tich phan
	float output_max;	//gioi han dau ra
} PID_t;

void PID_Init(PID_t *pid, float Kp, float Ki, float Kd, float integral_max, float output_max);
float PID_Compute(PID_t *pid, float setpoint, float measurement, float dt);
void PID_Reset(PID_t *pid);

#endif
