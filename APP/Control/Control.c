//------------------------------------------------------------------------------------------
//	*copyright		: L
//	*FileName		: control
//	*Author			: Cosmos
//	*Version		: V1.0
//	*Date			: 2018.9.12
//	*Description	: 飞行器姿态PID控制
//		
//	*Others			:
//	*Fun List		:
//	*History		:
//------------------------------------------------------------------------------------------
//Includes
//------------------------------------------------------------------------------------------
#include "APP/Control/Control.h"
#include "APP/Globle_Var/Globle_Var.h"
#include "Drv_PWM/Drv_PWM.h"
//------------------------------------------------------------------------------------------
PID_Typedef pitch_angle_PID, pitch_speed_PID;		//俯仰角度，角速度控制环
PID_Typedef roll_angle_PID, roll_speed_PID;			//翻滚角度，角速度控制环
PID_Typedef	yaw_angle_PID, yaw_speed_PID;			//偏航角度，角速度控制换
PID_Typedef height_PID;
#define		ERROR_ANGLE_PITCH_MAX			100
#define		ERROR_ANGLE_ROLL_MAX			100
#define		ERROR_ANGLE_YAW_MAX				100
#define		ERROR_SPEED_PITCH_MAX			100
#define		ERROR_SPEED_ROLL_MAX			100
#define		ERROR_SPEED_YAW_MAX				100
int16_t thr,pitch, roll, yaw;
//------------------------------------------------------------------------------------------
//	*Function	 : PID_Caculate
//	*Descriptiton: 计算PID结果
//	*Parameter	 : PID : 需要计算的PID结构体
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
float PID_Caculate(PID_Typedef PID)
{
	return PID.P*PID.Error + PID.I*PID.Integ + PID.D*PID.Deriv;
}
//------------------------------------------------------------------------------------------
//	*Function	 : PID_Init
//	*Descriptiton: 比例，微分，积分系数初始化
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void PID_Init(void)
{
	//俯仰PID系数初始化
	pitch_angle_PID.P = 4;
	pitch_angle_PID.I = 3;
	pitch_angle_PID.D = 1;

	pitch_speed_PID.P = 0.45;
	pitch_speed_PID.I = 0;
	pitch_speed_PID.D = 1.8;
	//翻滚PID系数初始化
	roll_angle_PID.P = 4;
	roll_angle_PID.I = 3;
	roll_angle_PID.D = 1;

	roll_speed_PID.P = 0.4;
	roll_speed_PID.I = 0;
	roll_speed_PID.D = 1.2;
	//偏航PID系数初始化
	yaw_angle_PID.P = 3;
	yaw_angle_PID.I = 0;
	yaw_angle_PID.D = 0;

	yaw_speed_PID.P = 3;
	yaw_speed_PID.I = 0;
	yaw_speed_PID.D = 0;
}
//------------------------------------------------------------------------------------------
//	*Function	 : control_angle
//	*Descriptiton: 角度控制环
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void control_angle(void)
{
	//角度误差
	pitch_angle_PID.Error = RC_Data.pitch - MPU_Data.pitch;
	roll_angle_PID.Error = RC_Data.roll - MPU_Data.roll;
	yaw_angle_PID.Error = RC_Data.yaw - MPU_Data.yaw;
	//角度积分
	pitch_angle_PID.Integ += pitch_angle_PID.Error;
	roll_angle_PID.Integ += roll_angle_PID.Error;
	yaw_angle_PID.Integ += yaw_angle_PID.Error;
	//积分幅值限定
	if (pitch_angle_PID.Integ > ERROR_ANGLE_PITCH_MAX)
		pitch_angle_PID.Integ = ERROR_ANGLE_PITCH_MAX;
	if (roll_angle_PID.Integ > ERROR_ANGLE_ROLL_MAX)
		roll_angle_PID.Integ = ERROR_ANGLE_ROLL_MAX;
	if (yaw_angle_PID.Integ > ERROR_ANGLE_YAW_MAX)
		yaw_angle_PID.Integ = ERROR_ANGLE_YAW_MAX;
	//角度微分
	pitch_angle_PID.Deriv = pitch_angle_PID.Error - pitch_angle_PID.PreError;
	roll_angle_PID.Deriv = roll_angle_PID.Error - roll_angle_PID.PreError;
	yaw_angle_PID.Deriv = yaw_angle_PID.Error - yaw_angle_PID.PreError;
	//角度PID输出---外环输出
	pitch_angle_PID.Output = PID_Caculate(pitch_angle_PID);
	roll_angle_PID.Output = PID_Caculate(roll_angle_PID);
	yaw_angle_PID.Output = PID_Caculate(yaw_angle_PID);
	//保存误差
	pitch_angle_PID.PreError = pitch_angle_PID.Error;
	roll_angle_PID.PreError = roll_angle_PID.Error;
	yaw_angle_PID.PreError = yaw_angle_PID.Error;
}
//------------------------------------------------------------------------------------------
//	*Function	 : control_speed
//	*Descriptiton: 角速度控制环
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void control_speed(void)
{
	//角速度误差
	roll_speed_PID.Error = gyro.x - roll_angle_PID.Output;
	pitch_speed_PID.Error = gyro.y - pitch_speed_PID.Output;
	yaw_speed_PID.Error = gyro.z - yaw_speed_PID.Output;
	//角度积分
	roll_speed_PID.Integ = (roll_speed_PID.Integ + roll_speed_PID.Error)/1000;
	pitch_speed_PID.Integ = (pitch_speed_PID.Integ+pitch_speed_PID.Error)/1000;
	yaw_speed_PID.Integ = (yaw_speed_PID.Integ+yaw_speed_PID.Error)/1000;
	//积分幅值限定
	if (pitch_speed_PID.Integ > ERROR_SPEED_PITCH_MAX)
		pitch_speed_PID.Integ = ERROR_SPEED_PITCH_MAX;
	if (roll_speed_PID.Integ > ERROR_SPEED_ROLL_MAX)
		roll_speed_PID.Integ = ERROR_SPEED_ROLL_MAX;
	if (yaw_speed_PID.Integ > ERROR_SPEED_YAW_MAX)
		yaw_speed_PID.Integ = ERROR_SPEED_YAW_MAX;
	//角速度微分
	pitch_speed_PID.Deriv = pitch_speed_PID.Error - pitch_speed_PID.PreError;
	roll_speed_PID.Deriv = roll_speed_PID.Error - roll_speed_PID.PreError;
	yaw_speed_PID.Deriv = yaw_speed_PID.Error - yaw_speed_PID.PreError;
	//角速度PID输出---外环输出
	pitch=pitch_speed_PID.Output = PID_Caculate(pitch_speed_PID);
	roll=roll_speed_PID.Output = PID_Caculate(roll_speed_PID);
	yaw=yaw_speed_PID.Output = PID_Caculate(yaw_speed_PID);
	//保存误差
	pitch_speed_PID.PreError = pitch_speed_PID.Error;
	roll_speed_PID.PreError = roll_speed_PID.Error;
	yaw_speed_PID.PreError = yaw_speed_PID.Error;

}
//------------------------------------------------------------------------------------------
//	*Function	 : control_height
//	*Descriptiton: 高度控制环
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void control_height(void)
{

}
//------------------------------------------------------------------------------------------
//	*Function	 : control
//	*Descriptiton: 飞行器姿态控制
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void control(void)
{
	int16_t motor[4];
	if (fly_flag.unclock)
	{
		control_angle();
		control_speed();
		//if (fly_flag.height_mod)
		//{
		//	control_height();
		//	//修改油门值
		//}
		//else
		//{
		//	thr = RC_Data.thr;
		//}
		motor[2] = (int16_t)(thr + pitch - roll - yaw);
		motor[0] = (int16_t)(thr - pitch + roll - yaw);
		motor[3] = (int16_t)(thr + pitch + roll + yaw);
		motor[1] = (int16_t)(thr - pitch - roll + yaw);
		MotorFlash(motor[0], motor[1], motor[2], motor[3]);
	}
	else
	{
		MotorFlash(0, 0, 0, 0);
	}
}
//------------------------------------------------------------------------------------------
