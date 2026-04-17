/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "icm42688.h"
#include <stdio.h>
extern SPI_HandleTypeDef hspi4;
#include "madgwick_filter.h"
#include "drv_rc.h"
#include "drv_motor.h"
#include "flight_state.h"
#include "mixer.h"
#include <string.h>
extern UART_HandleTypeDef huart1;

uint8_t rx_byte;
char rx_buffer[128];
uint8_t rx_index = 0;
uint8_t new_cmd_received = 0;

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
ICM42688_Data_t imu_data;
Madgwick_t madgwick;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for Task_System */
osThreadId_t Task_SystemHandle;
const osThreadAttr_t Task_System_attributes = {
  .name = "Task_System",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Task_FlightCont */
osThreadId_t Task_FlightContHandle;
const osThreadAttr_t Task_FlightCont_attributes = {
  .name = "Task_FlightCont",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for Task_RC_Update */
osThreadId_t Task_RC_UpdateHandle;
const osThreadAttr_t Task_RC_Update_attributes = {
  .name = "Task_RC_Update",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartTask_System(void *argument);
void StartTask_FlightControl(void *argument);
void StartTask_RC(void *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of Task_System */
  Task_SystemHandle = osThreadNew(StartTask_System, NULL, &Task_System_attributes);

  /* creation of Task_FlightCont */
  Task_FlightContHandle = osThreadNew(StartTask_FlightControl, NULL, &Task_FlightCont_attributes);

  /* creation of Task_RC_Update */
  Task_RC_UpdateHandle = osThreadNew(StartTask_RC, NULL, &Task_RC_Update_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartTask_System */
/**
  * @brief  Function implementing the Task_System thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartTask_System */
void StartTask_System(void *argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartTask_System */
  HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
  /* Infinite loop */
  for(;;)
  {
	  if (new_cmd_received == 1){
		  if (strncmp(rx_buffer, "PID:", 4) == 0){
			  float pR, iR, dR, pP, iP, dP, pY, iY, dY;

			  // Chặt 9 con số từ chuỗi
			  int parsed = sscanf(rx_buffer, "PID:%f,%f,%f,%f,%f,%f,%f,%f,%f",
								&pR, &iR, &dR, &pP, &iP, &dP, &pY, &iY, &dY);
			  // Nếu cắt đúng đủ 9 tham số thì mới cập nhật (Chống lỗi gửi thiếu)
			  if (parsed == 9) {
				  PID_Init(&pid_roll,  pR, iR, dR, 100.0f, 400.0f, 0.25f);
				  PID_Init(&pid_pitch, pP, iP, dP, 100.0f, 400.0f, 0.25f);
				  PID_Init(&pid_yaw,   pY, iY, dY, 100.0f, 400.0f, 0.0f);
			  }
		  }
		  new_cmd_received = 0;
	  }
	  osDelay(10);
  }
  /* USER CODE END StartTask_System */
}

/* USER CODE BEGIN Header_StartTask_FlightControl */
/**
* @brief Function implementing the Task_FlightCont thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask_FlightControl */
void StartTask_FlightControl(void *argument)
{
  /* USER CODE BEGIN StartTask_FlightControl */
	osDelay(500);

	//Init IMU
	ICM42688_Init(&hspi4);
	ICM42688_Calibrate();
	Madgwick_Init(&madgwick, 0.1f);


	//Init Motor - ESC 3s arm
	Motor_Init();
	FlightState_Init();
	Mixer_Init();
	flight_status.is_calibrated = 1;


	uint32_t last_tick = osKernelGetTickCount();
	const uint32_t loop_period = 1;
	float dt = 0.001f;

  /* Infinite loop */
  for(;;)
  {
	  ICM42688_Read_All(&imu_data);

	  Madgwick_Update(&madgwick,
	                  imu_data.gyro_x, imu_data.gyro_y, imu_data.gyro_z,
	                  imu_data.acc_x,  imu_data.acc_y,  imu_data.acc_z,
	                  dt);

	  if(flight_status.state == STATE_ARMED){
		  // Map RC (1000-2000) -> góc mục tiêu (-45 đến +45 độ)
		  float roll_target 	= ((float)rc_data.roll 	- 1500.0f) / 500.0f * 45.0f;
		  float pitch_target 	= ((float)rc_data.pitch - 1500.0f) / 500.0f * 45.0f;
		  float yaw_target 		= ((float)rc_data.yaw 	- 1500.0f) / 500.0f * 180.0f;

		  //Throttle expo
		  float t = (float)(rc_data.throttle - 1000) / 1000.0f;
		  if (t < 0.0f) t = 0.0f;
		  if (t > 1.0f) t = 1.0f;

		  float t_expo = t;
		  uint16_t throttle_final = (uint16_t)(t_expo * 1000.0f);

		  if (rc_data.throttle < 1010) {
		      throttle_final = 0;
		  } else if (throttle_final < 60) {
		      throttle_final = 60;
		  }


		  MotorOutput_t motors = Mixer_Compute(
				  	throttle_final,
					madgwick.roll, madgwick.pitch,
					imu_data.gyro_x, imu_data.gyro_y, imu_data.gyro_z,
					roll_target, pitch_target, yaw_target,
					dt);

		  // Map từ dải output của Mixer (0-1000) sang dải DShot vòng tua (48 - 2047)
		  uint16_t dshot_m1 = 48 + (motors.m1 * 1999) / 1000;
		  uint16_t dshot_m2 = 48 + (motors.m2 * 1999) / 1000;
		  uint16_t dshot_m3 = 48 + (motors.m3 * 1999) / 1000;
		  uint16_t dshot_m4 = 48 + (motors.m4 * 1999) / 1000;

		  // Gửi lệnh xuống Motor
		  Motor_SetDShot(MOTOR_1, dshot_m1);
		  Motor_SetDShot(MOTOR_2, dshot_m2);
		  Motor_SetDShot(MOTOR_3, dshot_m3);
		  Motor_SetDShot(MOTOR_4, dshot_m4);
	  } else {
		  Motor_Stop();
		  PID_Reset(&pid_roll);
		  PID_Reset(&pid_pitch);
		  PID_Reset(&pid_yaw);
		  PID_Reset(&pid_angle_roll);
		  PID_Reset(&pid_angle_pitch);
	  }

	  static uint16_t led_counter = 0;
	  if (++led_counter >= 500) {
	      HAL_GPIO_TogglePin(LED_ORANGE_GPIO_Port, LED_ORANGE_Pin);
	      led_counter = 0;
	  }

	  last_tick += loop_period;
	  osDelayUntil(last_tick);
  }
  /* USER CODE END StartTask_FlightControl */
}

/* USER CODE BEGIN Header_StartTask_RC */
/**
* @brief Function implementing the Task_RC_Update thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask_RC */
void StartTask_RC(void *argument)
{
  /* USER CODE BEGIN StartTask_RC */
	DRV_RC_Init();

  /* Infinite loop */
  for(;;)
  {
    DRV_RC_ParseData();
    DRV_RC_IsHealthy();
    FlightState_Update();

    float roll_target   = ((float)rc_data.roll  - 1500.0f) / 500.0f * 45.0f;
    float pitch_target  = ((float)rc_data.pitch - 1500.0f) / 500.0f * 45.0f;
    float yaw_target    = ((float)rc_data.yaw   - 1500.0f) / 500.0f * 180.0f;
    float vbat_mock     = 13.4f;

    static char tele_buf[128];
	int tele_len = snprintf(tele_buf, sizeof(tele_buf), "DAT:%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\r\n",
														rc_data.throttle,
														madgwick.roll, madgwick.pitch, madgwick.yaw,
														roll_target, pitch_target, yaw_target,
														vbat_mock);
	if (tele_len > 0) {
	        HAL_UART_Transmit_IT(&huart1, (uint8_t *)tele_buf, tele_len);
	}

    osDelay(20);
  }
  /* USER CODE END StartTask_RC */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if (huart->Instance == USART1) {
	        // Nếu nhận được dấu xuống dòng -> Kết thúc câu lệnh
	        if (rx_byte == '\n') {
	            rx_buffer[rx_index] = '\0'; // Khóa chuỗi
	            new_cmd_received = 1;       // Phất cờ báo cho Task_System biết
	            rx_index = 0;               // Reset con trỏ cho câu lệnh tiếp theo
	        }
	        else {
	            // Chống tràn bộ đệm
	            if (rx_index < 127) {
	                rx_buffer[rx_index++] = rx_byte;
	            }
	        }
	        // Kích hoạt lại "đôi tai" ngắt để hứng ký tự tiếp theo
	        HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
	    }
}

/* USER CODE END Application */

