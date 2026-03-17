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
  /* Infinite loop */
  for(;;)
  {
   osDelay(1);
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
	ICM42688_Init(&hspi4);
	ICM42688_Calibrate();



	extern float gyroX_offset, gyroY_offset, gyroZ_offset;
	printf("Offsets: X=%.3f Y=%.3f Z=%.3f\r\n",
	       gyroX_offset, gyroY_offset, gyroZ_offset);

	Madgwick_Init(&madgwick, 0.1f);

	uint32_t last_tick = osKernelGetTickCount();
	const uint32_t loop_period = 10;
	float dt = 0.01f;
  /* Infinite loop */
  for(;;)
  {
	  ICM42688_Read_All(&imu_data);

	  Madgwick_Update(&madgwick,
	                  imu_data.gyro_x, imu_data.gyro_y, imu_data.gyro_z,
	                  imu_data.acc_x,  imu_data.acc_y,  imu_data.acc_z,
	                  dt);


//	  printf("Roll:%5.1f Pitch:%5.1f Yaw:%5.1f\r\n",
//	             madgwick.roll, madgwick.pitch, madgwick.yaw);

	  HAL_GPIO_TogglePin(LED_ORANGE_GPIO_Port, LED_ORANGE_Pin);

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

    //debug
    printf("RC T:%d R:%d P:%d Y:%d FS:%d\r\n",
                   rc_data.throttle, rc_data.roll,
                   rc_data.pitch, rc_data.yaw,
                   rc_data.is_failsafe);
    osDelay(20);
  }
  /* USER CODE END StartTask_RC */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

