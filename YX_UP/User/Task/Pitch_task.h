#ifndef PITCH_TASK_H
#define PITCH_TASK_H
#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "remote_control.h"
#include "PID.h"
#include "arm_math.h"
#include "can.h"
#include "INS_task.h"
#include "Exchange_task.h"
#include "Can_user.h"
#include "stm32f4xx_it.h"

//===============================================宏定义================================================//

#define Up_inf -10	//imu抬头限位值
#define Down_inf 20	//imu低头限位值
#define mouse_y_valve 10.f	//鼠标死区阈值
#define mouse_y_weight 12.0f	//鼠标映射权重
#define Pitch_minipc_weight	0.5f	//视觉瞄准速度环权重
#define Pitch_sita_minipc_weight 0.002f	//视觉瞄准位置环权重
#define Pitch_sita_weight 0.2f	//遥控器控制位置环权重
#define Pitch_up 4150	//编码器抬头限位
#define Pitch_down 3700	//编码器低头限位

void Pitch_task(void const * argument);
extern fp32 INS_angle[3];//不知道为啥我在INS_task.h中明明已经声明过了却找不到
extern fp32 INS_gyro[3];
#ifdef __cplusplus
}
#endif

#endif