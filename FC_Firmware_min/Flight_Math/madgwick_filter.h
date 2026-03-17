#ifndef MADGWICK_FILTER_H_
#define MADGWICK_FILTER_H_

typedef struct {
    float q0, q1, q2, q3;  // Quaternion
    float beta;             // Filter gain
    float roll, pitch, yaw; // Góc Euler (độ)
} Madgwick_t;

void Madgwick_Init(Madgwick_t *mw, float beta);
void Madgwick_Update(Madgwick_t *mw,
                     float gx, float gy, float gz,
                     float ax, float ay, float az,
                     float dt);

#endif
