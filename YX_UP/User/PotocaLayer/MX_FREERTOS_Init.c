#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "MX_FREERTOS_Init.h"
#include "Yaw_task.h"
#include "StartDefaultTask.h"
#include "Pitch_task.h"
#include "Exchange_task.h"
#include "Friction_task.h"
#include "INS_task.h"

//================================================任务创建================================================//
osThreadId insTaskHandle;
osThreadId yawTaskHandle;
osThreadId defaultTaskHandle;
osThreadId pitchtaskHandle;
osThreadId exchangeHandle;
osThreadId frictionHandle;

void MX_FREERTOS_Init(void) {
  
	//测试任务
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
	
	//YAW控制任务
	osThreadDef(yawtask, Yaw_task, osPriorityRealtime, 0, 128);		
  yawTaskHandle = osThreadCreate(osThread(yawtask), NULL);
	
	//Pitch控制任务
	osThreadDef(pitchtask, Pitch_task, osPriorityRealtime, 0, 128);
  pitchtaskHandle = osThreadCreate(osThread(pitchtask), NULL);
	
	//上下C板通信任务
<<<<<<< HEAD
	osThreadDef(exchangetask, Exchange_task, osPriorityIdle, 0, 512);
=======
	osThreadDef(exchangetask, Exchange_task, osPriorityIdle, 0, 256);
>>>>>>> parent of 52bf27f (淇敼浜嗗拰瑙嗚鐨勬帴鍙�)
  exchangeHandle = osThreadCreate(osThread(exchangetask), NULL);

	//摩擦轮和拨盘控制任务
  osThreadDef(frictiontask, Friction_task, osPriorityIdle, 0, 128);
  frictionHandle = osThreadCreate(osThread(frictiontask), NULL);
	
	//六轴IMU任务
	osThreadDef(imutask, INS_Task,  osPriorityRealtime, 0, 1024);
  insTaskHandle = osThreadCreate(osThread(imutask), NULL);
	
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}