#include "icm42688.h"
#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>

static SPI_HandleTypeDef *imu_spi;
float gyroX_offset = 0, gyroY_offset = 0, gyroZ_offset = 0;

uint8_t ICM42688_Init(SPI_HandleTypeDef *hspi){
    imu_spi = hspi;

    HAL_GPIO_WritePin(ICM42688_CS_PORT, ICM42688_CS_PIN, GPIO_PIN_SET);
    HAL_Delay(100);


    SPI_Write_Register(imu_spi, ICM42688_CS_PORT, ICM42688_CS_PIN, 0x76, 0x00);
    HAL_Delay(10);

    // 2. Đọc thử ID
    uint8_t who_am_i = SPI_Read_Register(imu_spi, ICM42688_CS_PORT, ICM42688_CS_PIN, 0x75);

    if(who_am_i == 0x47){
        SPI_Write_Register(imu_spi, ICM42688_CS_PORT, ICM42688_CS_PIN, 0x4E, 0x0F);
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
	data->acc_x = (float)data->acc_x_raw / 2048.0f;
	data->acc_y = (float)data->acc_y_raw / 2048.0f;
	data->acc_z = (float)data->acc_z_raw / 2048.0f;

	data->gyro_x = ((float)data->gyro_x_raw / 16.4f) - gyroX_offset;
	data->gyro_y = ((float)data->gyro_y_raw / 16.4f) - gyroY_offset;
	data->gyro_z = ((float)data->gyro_z_raw / 16.4f) - gyroZ_offset;

}


void ICM42688_Calibrate(void){
	ICM42688_Data_t temp_data;
	float sumX = 0, sumY = 0, sumZ = 0;
	int samples = 500;

	printf("Calibrating IMU.... Please do not move!\r\n");
	for(int i = 0; i < samples; i++){
		ICM42688_Read_All(&temp_data);
		sumX += temp_data.gyro_x;
		sumY += temp_data.gyro_y;
		sumZ += temp_data.gyro_z;

		osDelay(2);
	}

	//trung binh sai so
	gyroX_offset = sumX / samples;
	gyroY_offset = sumY / samples;
	gyroZ_offset = sumZ / samples;

	printf("Calibration Done!\r\n");
	printf("Offsets -> X:%.2f, Y:%.2f, Z:%.2f\r\n",gyroX_offset, gyroY_offset, gyroZ_offset);
}
