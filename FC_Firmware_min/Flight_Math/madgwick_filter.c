#include "madgwick_filter.h"
#include <math.h>

void Madgwick_Init(Madgwick_t *mw, float beta){
    mw->q0   = 1.0f;
    mw->q1   = 0.0f;
    mw->q2   = 0.0f;
    mw->q3   = 0.0f;
    mw->beta = beta;  // thường dùng 0.1f
}

void Madgwick_Update(Madgwick_t *mw,
                     float gx, float gy, float gz,
                     float ax, float ay, float az,
                     float dt)
{
    float q0=mw->q0, q1=mw->q1, q2=mw->q2, q3=mw->q3;
    float recipNorm;
    float s0, s1, s2, s3;
    float qDot0, qDot1, qDot2, qDot3;
    float _2q0, _2q1, _2q2, _2q3;
    float _4q0, _4q1, _4q2, _4q3;
    float _8q1, _8q2;
    float q0q0, q1q1, q2q2, q3q3;

    // Gyro sang rad/s
    gx *= 0.0174533f;
    gy *= 0.0174533f;
    gz *= 0.0174533f;

    // Đạo hàm quaternion từ gyro
    qDot0 = 0.5f * (-q1*gx - q2*gy - q3*gz);
    qDot1 = 0.5f * ( q0*gx + q2*gz - q3*gy);
    qDot2 = 0.5f * ( q0*gy - q1*gz + q3*gx);
    qDot3 = 0.5f * ( q0*gz + q1*gy - q2*gx);

    // Normalize accel
    recipNorm = ax*ax + ay*ay + az*az;
    if(recipNorm == 0.0f) goto integrate;
    recipNorm = 1.0f / sqrtf(recipNorm);
    ax *= recipNorm;
    ay *= recipNorm;
    az *= recipNorm;

    // Gradient descent
    _2q0 = 2.0f*q0; _2q1 = 2.0f*q1;
    _2q2 = 2.0f*q2; _2q3 = 2.0f*q3;
    _4q0 = 4.0f*q0; _4q1 = 4.0f*q1;
    _4q2 = 4.0f*q2; _4q3 = 4.0f*q3;
    _8q1 = 8.0f*q1; _8q2 = 8.0f*q2;
    q0q0 = q0*q0; q1q1 = q1*q1;
    q2q2 = q2*q2; q3q3 = q3*q3;

    s0 = _4q0*q2q2 + _2q2*ax + _4q0*q1q1 - _2q1*ay;
    s1 = _4q1*q3q3 - _2q3*ax + 4.0f*q0q0*q1 - _2q0*ay - _4q1 + _8q1*q1q1 + _8q1*q2q2 + _4q1*az;
    s2 = 4.0f*q0q0*q2 + _2q0*ax + _4q2*q3q3 - _2q3*ay - _4q2 + _8q2*q1q1 + _8q2*q2q2 + _4q2*az;
    s3 = 4.0f*q1q1*q3 - _2q1*ax + 4.0f*q2q2*q3 - _2q2*ay;

    recipNorm = 1.0f / sqrtf(s0*s0 + s1*s1 + s2*s2 + s3*s3);
    s0 *= recipNorm; s1 *= recipNorm;
    s2 *= recipNorm; s3 *= recipNorm;

    qDot0 -= mw->beta * s0;
    qDot1 -= mw->beta * s1;
    qDot2 -= mw->beta * s2;
    qDot3 -= mw->beta * s3;

integrate:
    q0 += qDot0 * dt;
    q1 += qDot1 * dt;
    q2 += qDot2 * dt;
    q3 += qDot3 * dt;

    // Normalize quaternion
    recipNorm = 1.0f / sqrtf(q0*q0 + q1*q1 + q2*q2 + q3*q3);
    mw->q0 = q0 * recipNorm;
    mw->q1 = q1 * recipNorm;
    mw->q2 = q2 * recipNorm;
    mw->q3 = q3 * recipNorm;

    // Quaternion → Euler (độ)
    mw->roll  = atan2f(2.0f*(mw->q0*mw->q1 + mw->q2*mw->q3),
                       1.0f - 2.0f*(mw->q1*mw->q1 + mw->q2*mw->q2)) * 57.2958f;
    mw->pitch = asinf(2.0f*(mw->q0*mw->q2 - mw->q3*mw->q1)) * 57.2958f;
    mw->yaw   = atan2f(2.0f*(mw->q0*mw->q3 + mw->q1*mw->q2),
                       1.0f - 2.0f*(mw->q2*mw->q2 + mw->q3*mw->q3)) * 57.2958f;
}
