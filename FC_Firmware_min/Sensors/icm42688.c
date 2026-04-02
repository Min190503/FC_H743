#include "icm42688.h"
#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>

static SPI_HandleTypeDef *imu_spi;
IMU_Calib_t imu_calib =  {0};


uint8_t ICM42688_Init(SPI_HandleTypeDef *hspi){
    imu_spi = hspi;

    HAL_GPIO_WritePin(ICM42688_CS_PORT, ICM42688_CS_PIN, GPIO_PIN_SET);
    osDelay(100);


    SPI_Write_Register(imu_spi, ICM42688_CS_PORT, ICM42688_CS_PIN, 0x76, 0x00);
    osDelay(10);

    // 2. Đọc thử ID
    uint8_t who_am_i = SPI_Read_Register(imu_spi, ICM42688_CS_PORT, ICM42688_CS_PIN, 0x75);

    if(who_am_i == 0x47){
    	//DLPF
    	//Bat Gyro + Accel Low Noise mode
    	SPI_Write_Register(imu_spi, ICM42688_CS_PORT, ICM42688_CS_PIN, 0x4E, 0x0F);

    	//GYRO_CONFIGO: ODR 1kHz, 2000dps
    	SPI_Write_Register(imu_spi, ICM42688_CS_PORT, ICM42688_CS_PIN, 0x4F, 0x06);

    	//ACCEL_CONFIGO: ODR 1kHz, 16g
    	SPI_Write_Register(imu_spi, ICM42688_CS_PORT, ICM42688_CS_PIN, 0x50, 0x06);

    	//GYRO_ACCEL_CONFIGO: DLPE BW = 42Hz
        SPI_Write_Register(imu_spi, ICM42688_CS_PORT, ICM42688_CS_PIN, 0x52, 0x44);

        osDelay(50);
        return 1; // SUCCESS
    } else {
        return 0; // FAILED
    }
}


void ICM42688_Read_All(ICM42688_Data_t *data){
	uint8_t raw_buf[12];
	uint8_t reg = 0x1F | 0x80;		//thanh ghi Accel_Data_x1 + Bit Read

	HAL_GPIO_WritePin(ICM42688_CS_PORT, ICM42688_CS_PIN, GPIO_PIN_RESET);
	HAL_SPI_Transmit(imu_spi, &reg, 1, 10);
	HAL_SPI_Receive(imu_spi, raw_buf, 12, 10);
	HAL_GPIO_WritePin(ICM42688_CS_PORT, ICM42688_CS_PIN, GPIO_PIN_SET);


	//ghep byte du lieu tho
	data->acc_x_raw = (int16_t)((raw_buf[0] << 8) | raw_buf[1]);
	data->acc_y_raw = (int16_t)((raw_buf[2] << 8) | raw_buf[3]);
	data->acc_z_raw = (int16_t)((raw_buf[4] << 8) | raw_buf[5]);

	data->gyro_x_raw = (int16_t)((raw_buf[6] << 8) | raw_buf[7]);
	data->gyro_y_raw = (int16_t)((raw_buf[8] << 8) | raw_buf[9]);
	data->gyro_z_raw = (int16_t)((raw_buf[10] << 8) | raw_buf[11]);


	// Chuyển đổi sang đơn vị vật lý (Giả định dải đo mặc định: Accel +-16g, Gyro +-2000dps)
	// Công thức: Value = Raw / Sensitivity
	data->gyro_x = (data->gyro_x_raw / 16.4f)   - imu_calib.gyro_x_offset;
	data->gyro_y = (data->gyro_y_raw / 16.4f)   - imu_calib.gyro_y_offset;
	data->gyro_z = (data->gyro_z_raw / 16.4f)   - imu_calib.gyro_z_offset;
	data->acc_x  = (data->acc_x_raw  / 2048.0f) - imu_calib.acc_x_offset;
	data->acc_y  = (data->acc_y_raw  / 2048.0f) - imu_calib.acc_y_offset;
	data->acc_z  = (data->acc_z_raw  / 2048.0f) - imu_calib.acc_z_offset;

}


void ICM42688_Calibrate(void){
	ICM42688_Data_t temp;
	float gx = 0, gy = 0, gz = 0;
	float ax = 0, ay = 0, az = 0;
	int samples = 2000;

	printf("Calibrating...Do not move!\r\n");
	for(int i = 0; i < samples; i++){
		ICM42688_Read_All(&temp);
		gx += temp.gyro_x_raw / 16.4f;
		gy += temp.gyro_y_raw / 16.4f;
		gz += temp.gyro_z_raw / 16.4f;
		ax += temp.acc_x_raw  / 2048.0f;
		ay += temp.acc_y_raw  / 2048.0f;
		az += temp.acc_z_raw  / 2048.0f;

		osDelay(1);
	}

	imu_calib.gyro_x_offset = gx / samples;
	imu_calib.gyro_y_offset = gy / samples;
	imu_calib.gyro_z_offset = gz / samples;

	imu_calib.acc_x_offset = ax / samples;
	imu_calib.acc_y_offset = ay / samples;
	imu_calib.acc_z_offset = (az / samples) - 1.0f;

	printf("Gyro offset: X=%.3f Y=%.3f Z=%.3f\r\n",
	           imu_calib.gyro_x_offset,
	           imu_calib.gyro_y_offset,
	           imu_calib.gyro_z_offset);

	printf("Acc  offset: X=%.3f Y=%.3f Z=%.3f\r\n",
	           imu_calib.acc_x_offset,
	           imu_calib.acc_y_offset,
	           imu_calib.acc_z_offset);
}
