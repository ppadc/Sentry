#include "PID.h"


//PID��ʼ��
void pid_init(pid_struct_t *pid,
              float kp,
              float ki,
              float kd,
              float i_max,
              float out_max)
{
  pid->kp      = kp;
  pid->ki      = ki;
  pid->kd      = kd;
  pid->i_max   = i_max;
  pid->out_max = out_max;
}

float pid_calc(pid_struct_t *pid, float ref, float fdb)//ref��Ŀ��ֵ,fdb�ǵ��������ٶȷ���ֵ
{
  pid->ref = ref;
  pid->fdb = fdb;
  pid->err[1] = pid->err[0];//err[1]����һ�μ�������Ĳ�ֵ
  pid->err[0] = pid->ref - pid->fdb;//err[0]����һ�ε�Ԥ���ٶȺ�ʵ���ٶȵĲ�ֵ,������ֵ�ǿ����Ǹ�����
  
  pid->p_out  = pid->kp * pid->err[0];//40 3 0�Ǳ�׼ֵ��������ӵ�watch1����
  pid->i_out += pid->ki * pid->err[0];
  pid->d_out  = pid->kd * (pid->err[0] - pid->err[1]);
  LIMIT_MIN_MAX(pid->i_out, -pid->i_max, pid->i_max);//��ֹԽ��
  
  pid->output = pid->p_out + pid->i_out + pid->d_out;
  LIMIT_MIN_MAX(pid->output, -pid->out_max, pid->out_max);//��ֹԽ��
  return pid->output;//������صı�����ת�ٺ�ת�ص���������ֻ�ܷ���ѹֵ(-30000��30000)���е�������PID
}

float pid_calc_sita(pid_struct_t *pid,float ref, float fdb) //λ�û�PID���㣬��λ�ǽǶȣ�ref�ǽǶ�Ŀ��ֵ��fdb�ǽǶȷ���ֵ
{
  pid->ref = ref;
  pid->fdb = fdb;
	pid->err[1] = pid->err[0];
	
	float err = 0;
	err = ref - fdb;
	if(err > 180)
	{
		err -= 360;
	}
	else if(err < -180)
	{
		err += 360;
	}
  pid->err[0] = err;
	
  pid->p_out  = pid->kp * pid->err[0];
  pid->i_out += pid->ki * pid->err[0];
  pid->d_out  = pid->kd * (pid->err[0] - pid->err[1]);
	
  LIMIT_MIN_MAX(pid->i_out, -pid->i_max, pid->i_max);//��ֹiֵ����Խ��
  
  pid->output = pid->p_out + pid->i_out + pid->d_out;
  LIMIT_MIN_MAX(pid->output, -pid->out_max, pid->out_max);//��ֹ����Խ��
  return pid->output;
}