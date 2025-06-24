/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
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
#include "cmsis_os.h"
#include "main.h"
#include "task.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "platform.h"

#include "usb_host.h"

#include "SEGGER_SYSVIEW.h"
#include "fonas/fonas.h"
#include "fonas/logger/logger.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

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
/* Definitions for startupTask */
osThreadId_t startupTaskHandle;
const osThreadAttr_t startupTask_attributes = {
    .name = "startup",
    .stack_size = 8 * 1024,
    .priority = (osPriority_t)osPriorityRealtime,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void startupTask(void *argument);

extern void MX_USB_HOST_Init(void);
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
  /* creation of startupTask */
  startupTaskHandle = osThreadNew(startupTask, NULL, &startupTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */
}

/* USER CODE BEGIN Header_StartupTask */
/**
 * @brief  Function implementing the startupTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartupTask */
void startupTask(void *arg) {
  UNUSED(arg);
  SEGGER_SYSVIEW_Start();
  /* init code for USB_HOST */
  MX_USB_HOST_Init();
  /* USER CODE BEGIN 5 */
  fonas_logger_enable();
  platform_app();
  //  static uint16_t audio_out_buffer[256];
  //  HAL_I2S_Transmit_DMA(&hi2s2, audio_out_buffer, 256);
  osThreadTerminate(startupTaskHandle);
  /* USER CODE END 5 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

#include "tim.h"
#include <stdio.h>
#include <string.h>

void freertos_config_configure_timer_for_run_time_stats() { HAL_TIM_Base_Start(&htim2); }

uint32_t freertos_config_get_run_time_counter_value() { return __HAL_TIM_GET_COUNTER(&htim2); }

const TickType_t FREERTOS_STATS_PRINT_PERIOD = pdMS_TO_TICKS(1000);
const uint32_t FREERTOS_STATS_MAX_LINE_LENGTH = 48;
#define PRINT_FREERTOS_STATS_ENABLED 0

void vApplicationIdleHook(void) {
#if PRINT_FREERTOS_STATS_ENABLED
  static uint32_t lastWakeTime = 0;
  if ((xTaskGetTickCount() - lastWakeTime) >= FREERTOS_STATS_PRINT_PERIOD) {
    lastWakeTime = xTaskGetTickCount();
    UBaseType_t numTasks = uxTaskGetNumberOfTasks();
    TaskStatus_t *taskStatusArray = pvPortMalloc(numTasks * sizeof(TaskStatus_t));
    if (!taskStatusArray) {
      return;
    }
    uint32_t ulTotalTime;
    numTasks = uxTaskGetSystemState(taskStatusArray, numTasks, &ulTotalTime);
    ulTotalTime /= 100UL;
    if (ulTotalTime == 0UL) {
      vPortFree(taskStatusArray);
      return;
    }
    const uint32_t buff_size = (1 + numTasks) * FREERTOS_STATS_MAX_LINE_LENGTH;
    char *buff = pvPortMalloc(buff_size);
    if (!buff) {
      vPortFree(taskStatusArray);
      return;
    }
    char *buff_ = buff;
    buff_ += snprintf(buff_, (buff_size - (buff_ - buff)), "Task              CPU %%  Stack %%\n");
    for (size_t i = 0; i < numTasks; i++) {
      const float cpuUsagePercentage = (float)taskStatusArray[i].ulRunTimeCounter / (float)ulTotalTime;
      extern uint32_t taskGetStackSizeWords(TaskHandle_t taskHandle);
      const uint32_t stackSizeWords = taskGetStackSizeWords(taskStatusArray[i].xHandle);
      const uint32_t stackUsedWords = stackSizeWords - uxTaskGetStackHighWaterMark2(taskStatusArray[i].xHandle);
      const float stackUsagePercentage = 100.0f * (float)stackUsedWords / (float)stackSizeWords;
      buff_ += snprintf(buff_, (buff_size - (buff_ - buff)), "%-16s  %5.2f  %5.2f\n", taskStatusArray[i].pcTaskName,
                        cpuUsagePercentage, stackUsagePercentage);
    }
    platform_print_freertos_stats(buff);
    vPortFree(taskStatusArray);
    vPortFree(buff);
  }
#endif
}

void vApplicationMallocFailedHook() { FONAS_PANIC(); }

/* USER CODE END Application */
