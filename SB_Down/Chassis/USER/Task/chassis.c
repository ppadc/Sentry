#include "freertos.h"
#include "can.h"
#include "chassis.h"
#include "cmsis_os.h"
#include "user_can.h"
#include "user_pid.h"
#include "rc_potocal.h"
#include "handle_value.h"
#include "motion_overlay.h"
#include "channel_changes.h"
#include "INS_task.h"

extern motor_info motor[8]; //底盘电机数据
extern RC_ctrl_t rc_ctrl; //遥控器数据
extern fp32 INS_angle[3]; //下C板陀螺仪数据
extern up_data UpData; //上C板数据
extern int16_t motor_angle[4]; //6020角度 在motion_overlay.c中计算 作为全局变量
extern int16_t motor_speed[4]; //3508速度

uint16_t initial_angle[4];
int16_t Max_out_a = 8192;
int16_t Max_iout_a = 8192;
int16_t Max_out_s = 16384; //电压控制转速，电流控制扭矩
int16_t Max_iout_s = 16384;
pidTypeDef PID_angle[4];
pidTypeDef PID_speed_3508[4];
pidTypeDef PID_speed_6020[4];
fp32 theta; //云台坐标系与底盘坐标系间夹角(此时为0~360度) 后期接收后需要对所得theta进行处理

void Yaw_Diff()
{
	theta = UpData.yaw_up - INS_angle[0];
}

void Chassis(void const * argument)
{
	float PID_s[3] = {30,0.01,0};
	float PID_a[3] = {6,0,0.01};
	float PID[3] = {5,0,0};
	
	int m = 0;
	
	for(int i=0;i<4;i++){
		pid_init(&PID_speed_6020[i],PID_s[0],PID_s[1],PID_s[2]);
		pid_init(&PID_angle[i],PID_a[0],PID_a[1],PID_a[2]);
		pid_init(&PID_speed_3508[i],PID[0],PID[1],PID[2]);
	}
	
  for(;;)
  {
//**************************************读取6020初始角度**************************************//
//		if(m==0){
//			HAL_Delay(10);
//			for(int i=0;i<4;i++)
//			{
//				initial_angle[i] = motor[i].angle; //读取电机初始角度 0~8192
//			}
//			m++;
//		}
//********************************************************************************************//

		//设置初始角度		
		if(m==0){
			initial_angle[0] = 7819; //初始角度（底盘正前方各轮子角度）
			initial_angle[1] = 1858;
			initial_angle[2] = 3805;
			initial_angle[3] = 5735;
			m++;
		}

		//遥控器控制底盘不同运动
		//具体实现方式在"motion_overlay.c"
		if(rc_ctrl.rc.s[0]==1) //右上拨杆为1（推到最上方）
		{
			translational_control(); //平移运动
		}
		else if(rc_ctrl.rc.s[0]==3) //右上拨杆为3（推到中间位置）
		{
			compound_control(); //旋转加平移运动
		}
		else if(rc_ctrl.rc.s[0]==2) //右上拨杆为2（推到最下方）
		{
			rotate_control(); //小陀螺旋转 无跟随 
		}
    osDelay(10);
  }
}