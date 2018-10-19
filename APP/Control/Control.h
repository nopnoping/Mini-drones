#ifndef __CONTROL_H__
#define __CONTROL_H__

typedef struct
{
	float P;			//比例系数
	float I;			//积分系数
	float D;			//微分系数
	//float Desired;		
	float Error;		//当前误差
	float PreError;		//前次误差
	double Integ;		//误差积分
	float Deriv;		//误差微分
	float Output;		//PID结果
}PID_Typedef;

void PID_Init(void);
void control(void);
#endif // !__CONTROL_H__
