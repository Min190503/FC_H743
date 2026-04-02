#ifndef ICM42688_H_
#define ICM42688_H_

#include "stdint.h"
#include "drv_spi.h"


#define ICM42688_CS_PORT	GPIOE
#define ICM42688_CS_PIN		GPIO_PIN_11

//Dia chi thanh ghi cua ICM42688
#define ICM42688_REG_WHO_AM_I	0x75
#define ICM42688_WHO_AM_I_VAL	0x47  //gia tri chuan neu ket noi thanh cong


//khoi tao
uint8_t ICM42688_Init(SPI_HandleTypeDef *hspi);

typedef struct {
	int16_t acc_x_raw, acc_y_raw, acc_z_raw;
	int16_t gyro_x_raw, gyro_y_raw, gyro_z_raw;


	float acc_x, acc_y, acc_z;
	float gyro_x, gyro_y, gyro_z;

	float temperature;
} ICM42688_Data_t;

typedef struct {
    float gyro_x_offset;
    float gyro_y_offset;
    float gyro_z_offset;
    float acc_x_offset;
    float acc_y_offset;
    float acc_z_offset;
} IMU_Calib_t;
extern IMU_Calib_t imu_calib;

uint8_t ICM42688_Init(SPI_HandleTypeDef *hspi);
void 	ICM42688_Read_All(ICM42688_Data_t *data);
void	ICM42688_Calibrate(void);


#endif
