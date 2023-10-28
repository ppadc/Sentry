#include "Exchange_task.h"
#include "SolveTrajectory.h"
//================================================通信任务================================================//



//================================================函数================================================//
//上C板向下C板发送数据
static void Up_send_to_down();

uint8_t vision_send[28];	//视觉接口数据帧
remote_flag_t remote;	//键盘按键读取


<<<<<<< HEAD
//获取键盘信息
static void Get_keyboard();
=======
extern RC_ctrl_t rc_ctrl;
//extern ins_data_t ins_data;
extern uint16_t Remember_pitch;
extern fp32 Err_pitch;
extern uint8_t Remember_pitch_flag;
extern fp32 ins_yaw;
uint8_t ins_buf[8];
uint16_t ins_buf_temp;		//define a temp to receive and change float to int16
 
>>>>>>> parent of 331e8e5 (上下位机通信修改)

//获取Nuc的信息
static void Get_minipc();

//判断上位机检测到目标，检测到就进行解算，没检测到赋0
static void Judge_minipc();

//对来自Nuc的信息进行解码
static void Vision_read(uint8_t rx_buffer[]);

//向Nuc发送信息
static void Stm_pc_send();

//弹道补偿API接口初始化
static void SolveTrajectory_Init();

//哨兵状态量及官方裁判系统数据初始化
static void Sentry_Init();

//================================================全局变量================================================//
extern UART_HandleTypeDef huart1;
volatile uint8_t rx_len_uart1 = 0;  //接收一帧数据的长度
volatile uint8_t recv_end_flag_uart1 = 0; //一帧数据接收完成标志
uint8_t rx_buffer[100]={0};  //接收数据缓存数组

uint8_t vision_send[28];	//视觉接口发送数据帧

Chase_t chase;	//赋予电机追踪的数据结构体
Vision_t vision;	//视觉数据发送结构体
vision_receive_t vision_receive;	//视觉数据接收结构体
remote_flag_t remote;	//键盘按键读取(结构体)
Sentry_t Sentry;	//哨兵状态量和裁判系统数据结构体

uint8_t ins_buf[8];

int16_t Yaw_minipc;
int16_t Pitch_minipc;
fp32 Yaw_minipc_fp;
fp32 Pitch_minipc_fp;
Vision_t vision;	//视觉通信结构体

void Get_keyboard();
void Get_minipc();
void remote_data_read(uint8_t rx_buffer[]);
void Stm_pc_send();


void Exchange_task(void const * argument)
{
  /* USER CODE BEGIN StartTask03 */
  /* Infinite loop */

	Sentry_Init();	//哨兵状态量及裁判系统数据初始化
	SolveTrajectory_Init();//初始化参数

	
	ins_buf[0] = 8;	//imu receive tag

	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE); //使能uart1的IDLE中断
	HAL_UART_Receive_DMA(&huart1,rx_buffer,100); //开启接收
	
  for(;;)
  {

		Up_send_to_down();	//上C向下C发送信息
		Get_keyboard();		//解算键盘的信息
		Get_minipc();		//取出nuc的信息
//		Judge_minipc();		//检测是否识别到目标，识别到目标就解算，没识别到赋0
		Stm_pc_send();		//向nuc发送信息


		can_remote(ins_buf,0x55);
		
		Get_keyboard();		//解算keyboard flag	
		Get_minipc();
		Stm_pc_send();


    osDelay(1);
  }
  /* USER CODE END StartTask03 */
} 

//================================================获取键盘数据================================================//
//注：can接收后会先储存在rc_ctrl这个结构体里(官方结构体)，这里将它取出来统一处理了
static void Get_keyboard()	
{
		memcpy(&remote.key , &rc_ctrl.key , 2);
		remote.mouse.press_left = rc_ctrl.mouse.press_l;
		remote.mouse.press_right = rc_ctrl.mouse.press_r;
	  remote.mouse.x = rc_ctrl.mouse.x;
		remote.mouse.y = rc_ctrl.mouse.y;
}

//================================================通信接收任务================================================//
static void Get_minipc()
{

		if(recv_end_flag_uart1 == 1)  //接收完成标志
		{			
			if(rx_buffer[0] == 0x01)
			{
				remote_data_read(rx_buffer);
			}
			
			recv_end_flag_uart1 = 0;//清除接收结束标志位
			for(uint8_t i=0;i<rx_len_uart1;i++)
				{
					rx_buffer[i]=0;//清接收缓存
				}
				//memset(rx_buffer,0,rx_len);
			rx_len_uart1 = 0;//清除计数
			HAL_UART_Receive_DMA(&huart1,rx_buffer,BUFFER_SIZE);//重新打开DMA接收
			
		}
}


//================================================通信读取解算任务================================================//
static void Vision_read(uint8_t rx_buffer[])
{}
//================================================通信解算任务================================================//
void remote_data_read(uint8_t rx_buffer[])

{
	Yaw_minipc = (int)(rx_buffer[1] << 8 | rx_buffer[2]);
	Pitch_minipc = (int)(rx_buffer[3] << 8 | rx_buffer[4]);
	Yaw_minipc_fp = (float)(Yaw_minipc * 100)/32767;
	Pitch_minipc_fp = (float)(Pitch_minipc * 100)/32767;	
	Yaw_minipc = (int)(Yaw_minipc * 100)/32767;
	Pitch_minipc = (int)(Pitch_minipc * 100)/32767;	
}

//================================================数据stm32 -> 上位机================================================//
static void Stm_pc_send()
{
<<<<<<< HEAD
	vision.header = 0x5A;
	vision.official.detect_color = 1;	//读取裁判系统数据判断红蓝方
	vision.official.reset_tracker = 0;
	vision.official.reserved = 6;
	vision.roll = INS_angle[2]/57.3f;
	vision.pitch = INS_angle[1]/57.3f;
	vision.yaw = INS_angle[0]/57.3f;
	vision.aim_x = 0.5;
	vision.aim_y = 0.5;
	vision.aim_z = 5;
	vision.checksum = 0xAAAA;	//CRC16校验，我没用，发了个定值做校验
	
	memcpy(&vision_send[0],&vision.header,1);
	memcpy(&vision_send[1],&vision.official,1);
	memcpy(&vision_send[2],&vision.roll,4);
	memcpy(&vision_send[6],&vision.pitch,4);
	memcpy(&vision_send[10],&vision.yaw,4);
	memcpy(&vision_send[14],&vision.aim_x,4);
	memcpy(&vision_send[18],&vision.aim_y,4);
	memcpy(&vision_send[22],&vision.aim_z,4);
	memcpy(&vision_send[26],&vision.checksum,2);
	
	HAL_UART_Transmit_DMA(&huart1,vision_send,28);
}


//================================================弹道补偿API接口================================================//
static void SolveTrajectory_Init()
{
		//    //定义参数
    st.k = 0.092;
    st.bullet_type =  BULLET_17;
    st.current_v = 18;
    st.current_pitch = 0;
    st.current_yaw = 0;
    st.xw = 3.0;
    // st.yw = 0.0159;
    st.yw = 0;
    // st.zw = -0.2898;
    st.zw = 1.5;

    st.vxw = 0;
    st.vyw = 0;
    st.vzw = 0;
    st.v_yaw = 0;
    st.tar_yaw = 0.09131;
    st.r1 = 0.5;
    st.r2 = 0.5;
    st.dz = 0.1;
    st.bias_time = 100;
    st.s_bias = 0.19133;
    st.z_bias = 0.21265;
    st.armor_id = ARMOR_INFANTRY3;
    st.armor_num = ARMOR_NUM_NORMAL;
		
}

//================================================上C向下C发送数据================================================//
static void Up_send_to_down()
{
		uint8_t ins_buf[8] = {0};
		ins_buf[0] = 8;	//	imu头帧标识
		memcpy(&ins_buf[1],&INS_angle[0],4); //获取yaw的角度并储存在发送的字节中
		can_remote(ins_buf,0x55);
}

//================================================视觉识别检测判断================================================//
//判断上位机检测到目标，检测到就进行解算，没检测到赋0
static void Judge_minipc()
{
		if(vision_receive.x && vision_receive.y && vision_receive.z)
		{
			autoSolveTrajectory(&vision.pitch, &vision.yaw, &vision.aim_x, &vision.aim_y, &vision.aim_z);	//弹道解算
			chase.pitch = vision.pitch * 180 / PI;
			chase.yaw = vision.yaw * 180 / PI;
			Sentry.foe_flag = 1;	//识别标志位
			Sentry.foe_count = 0;	//计数器清零
		}
		else
		{
			chase.pitch = 0;
			chase.yaw = 0;
		}
}

//================================================哨兵状态及裁判系统数据初始化================================================//
static void Sentry_Init()
{
	Sentry.foe_flag = 0;
	Sentry.foe_count = 0;
	Sentry.Flag_progress = 0;
	Sentry.Flag_judge = 0;
=======
	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_11,GPIO_PIN_SET);
	Temp_pc[0] = 0x10;
//	Temp_pc[1] = ins_data.angle[0];//云台Yaw角度
//	Temp_pc[2] = ins_data.angle[1];//云台Pitch角度
	Temp_pc[3] = 'a';//状态
	Temp_pc[4] = '1';
	Temp_pc[5] = '0';
	Temp_pc[6] = 0;
	Temp_pc[7] = '\n';
	//HAL_UART_Transmit_DMA(&huart1,Temp_pc,8);
>>>>>>> parent of 331e8e5 (上下位机通信修改)
}

