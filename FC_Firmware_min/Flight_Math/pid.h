#ifndef PID_H
#define PID_H

typedef struct {
	float Kp;
	float Ki;
	float Kd;

	float integral;				//tich luy sai so (I)
	float prev_measurement;		//chong kick
	float output;				//ket qua dau ra

	//PT1 Low_pass filter cho D_term
	float d_filtered;			//lich su bo loc
	float d_alpha;				//he so cat nhieu


	//Gioi han
	float integral_max; 		//chong tran tich phan
	float output_max;			//gioi han dau ra
} PID_t;

void PID_Init(PID_t *pid, float Kp, float Ki, float Kd, float integral_max, float output_max, float alpha);
float PID_Compute(PID_t *pid, float setpoint, float measurement, float dt);
void PID_Reset(PID_t *pid);

#endif
