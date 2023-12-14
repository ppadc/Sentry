#include "Yaw_task.h"
//================================================YAW������������================================================//

//	���ļ�������̨�������񣬻���һ����̨�ϵ�C������ϵ�����������̨������
//	����ͷ�õ�CAN2,ID��1��2�����ID��0���ұ�ID��1

//================================================ȫ�ֱ���================================================//
float target_yaw_left;	//�����Դ���Ŀ��yaw��������꣩
float target_yaw_right;
int16_t Init_encoder_left = 6818;		//���Դ���������ǰ����ʼֵ(��װ�ú�ֵ�̶�)
int16_t Init_encoder_right = 7154;		//���Դ�
float Yaw_left;	//����ʱ�����Դ���yaw��������꣩
float Yaw_right;	
float Yaw_left_c;	//����ʱ�����Դ���yaw���������꣩
float Yaw_right_c;	
//================================================����================================================//

//��ʼ��PID����
static void Yaw_init();	

//ÿ��ѭ����ʼ��
static void Yaw_loop_init();

//��ȡC��IMUֵ
static void Yaw_read_imu();

//�����Դ���λ�ÿ���ģʽ
static void Yaw_mode_remote_site();

//��ԽǶ�����
static void Yaw_restrict();

//�ٶȻ�����
static void Yaw_speed_calc();

//��ѹ������
static void Yaw_voltage_calc();

//ѭ����ʼ��
static void Yaw_loop_init();
//================================================YAW�����������================================================//
void Yaw_task(void const *pvParameters)
{
  //������ʼ������
	osDelay(2000);//�ϵ�ȴ�IMU�����ɹ�
	Yaw_init();
	
	//ѭ����������
  for(;;)
  {
		Yaw_loop_init();//ѭ����ʼ��
		Yaw_read_imu();//��ȡImu�Ƕ�
		Yaw_mode_remote_site();//λ�ÿ���ģʽ
		Yaw_restrict();//��ԽǶ�����
		Yaw_speed_calc();//�ٶȻ�����
		Yaw_voltage_calc();//��ѹ������
		Yaw_can_send();
    osDelay(1);
  }

}

//================================================YAW��PID������ʼ��================================================//
static void Yaw_init()
{
	pid_init(&motor_pid_can_2[0],30,0.001,0,30000,30000);
	pid_init(&motor_pid_sita_can_2[0],3,0,1,30000,30000);
	pid_init(&motor_pid_can_2[1],30,0.001,0,30000,30000);
	pid_init(&motor_pid_sita_can_2[1],3,0,1,30000,30000);
	
	int16_t Yaw_left_int = motor_value(Init_encoder_left,motor_info_can_2[0].rotor_angle);
	int16_t Yaw_right_int = motor_value(Init_encoder_right,motor_info_can_2[1].rotor_angle);
	Yaw_left = (float)Yaw_left_int;
	Yaw_right = (float)Yaw_right_int;
	
	target_yaw_left = Yaw_left;
	target_yaw_right = Yaw_right;
}

//================================================YAW��Ƕȶ�ȡ===============================================//
static void Yaw_read_imu()
{
	//INS_angle[0]��C�������ǵ�ֵ
	//ԭʼ����˳ʱ��Ϊ��
	//��������ʱ��Ϊ��(�������ϵ)
	//    0
	//180 -180
	int16_t Yaw_left_int = motor_value(Init_encoder_left,motor_info_can_2[0].rotor_angle);
	int16_t Yaw_right_int = motor_value(Init_encoder_right,motor_info_can_2[1].rotor_angle);
	Yaw_left = (float)Yaw_left_int;
	Yaw_right = (float)Yaw_right_int;
	
	//��C���ϵ���һ�̵�����ϵΪ������ϵ(��������ϵ)
	Yaw_left_c = Yaw_left + INS_angle[0];
	Yaw_right_c = Yaw_right + INS_angle[0];
	//Խ�紦����ûд����
	
}

//================================================λ�ÿ���ģʽ================================================//
static void Yaw_mode_remote_site()
{
		if(rc_ctrl.rc.ch[1] >= -660 && rc_ctrl.rc.ch[0]<= 660)
		{			
			target_yaw_left -= rc_ctrl.rc.ch[1]/660.0 * Yaw_sita_weight; 	
			target_yaw_right = -target_yaw_left;
		}
}

//================================================Yaw�����������================================================//
static void Yaw_can_send()
{
	CAN_TxHeaderTypeDef tx_header;
  uint8_t             tx_data[8];
	
	tx_header.StdId = 0x1ff;
  tx_header.IDE   = CAN_ID_STD;//��׼֡
  tx_header.RTR   = CAN_RTR_DATA;//����֡
  tx_header.DLC   = 8;		//�������ݳ��ȣ��ֽڣ�

	tx_data[0] = (motor_info_can_2[0].set_voltage>>8)&0xff;	//�ȷ��߰�λ		
  tx_data[1] = (motor_info_can_2[0].set_voltage)&0xff;
  tx_data[2] = (motor_info_can_2[1].set_voltage>>8)&0xff;	
  tx_data[3] = (motor_info_can_2[1].set_voltage)&0xff;
  tx_data[4] = 0x00;
  tx_data[5] = 0x00;
  tx_data[6] = 0x00;
  tx_data[7] = 0x00;
  HAL_CAN_AddTxMessage(&hcan2, &tx_header, tx_data,(uint32_t*)CAN_TX_MAILBOX1);
}

//================================================Yaw�Ƕ�����================================================//
static void Yaw_restrict()
{
	if(target_yaw_left<0)
	{
		target_yaw_left=0; 
	}
	else if(target_yaw_left>180)
	{
		target_yaw_left=180; 
	}
	
	if(target_yaw_right>0)
	{
		target_yaw_left=0; 
	}
	else if(target_yaw_left<-180)
	{
		target_yaw_left=-180; 
	}
}

//================================================�ٶȻ�������㣨��װ6020ȡ��ֵ��================================================//
static void Yaw_speed_calc()
{
	target_speed_can_2[0] -=  pid_calc_sita(&motor_pid_sita_can_2[0], target_yaw_left, Yaw_left);
	target_speed_can_2[1] -=  pid_calc_sita(&motor_pid_sita_can_2[1], target_yaw_right, Yaw_right);
}

//================================================��ѹ������================================================//
static void Yaw_voltage_calc()
{
	motor_info_can_2[0].set_voltage = pid_calc(&motor_pid_can_2[0], target_speed_can_2[0], motor_info_can_2[0].rotor_speed);
	motor_info_can_2[1].set_voltage = pid_calc(&motor_pid_can_2[1], target_speed_can_2[1], motor_info_can_2[1].rotor_speed);
}

//================================================ѭ����ʼ��================================================//
static void Yaw_loop_init()
{
	target_speed_can_2[0] = 0;
	target_speed_can_2[1] = 0;
}
	