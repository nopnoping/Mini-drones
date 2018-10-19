//------------------------------------------------------------------------------------------
//	*copyright		: L
//	*FileName		: Drv_MPU
//	*Author			: Cosmos
//	*Version		: V1.0
//	*Date			: 2018.9.6	 
//	*Description	: 六轴初始化
//		
//	*Others			:
//	*Fun List		:
//	*History		: 2018.9.24 测试成功
//------------------------------------------------------------------------------------------
//Includes
//------------------------------------------------------------------------------------------
#include "Drv_MPU/Drv_MPU.h"
#include "Drv_I2C/Drv_I2C.h"
#include "Drv_DMP/inv_mpu.h"
#include "Drv_DMP/inv_mpu_dmp_motion_driver.h"
#include "Drv_time/Drv_time.h"
#include <math.h>
//------------------------------------------------------------------------------------------
//	*Function	 : MPU_Init
//	*Descriptiton: 初始化MPU6050
//	*Parameter	 : None 
//	*Return		 : 0 : 成功    其他,错误代码
//	*Others		 :
//------------------------------------------------------------------------------------------
uint8_t MPU_Init(void)
{
	uint8_t res;
	I2C_My_Init();															//初始化IIC总线
	MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X80);	//复位MPU6050
	delay_ms(100);
	MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X00);	//唤醒MPU6050 
	MPU_Set_Gyro_Fsr(3);										//陀螺仪传感器,±2000dps
	MPU_Set_Accel_Fsr(0);										//加速度传感器,±2g
	MPU_Set_Rate(50);												//设置采样率50Hz
	MPU_Write_Byte(MPU_INT_EN_REG, 0X00);		//关闭所有中断
	MPU_Write_Byte(MPU_USER_CTRL_REG, 0X00);	//I2C主模式关闭（把MPU6050设置为从模式）
	MPU_Write_Byte(MPU_FIFO_EN_REG, 0X00);		//关闭FIFO
	MPU_Write_Byte(MPU_INTBP_CFG_REG, 0X80);	//INT引脚低电平有效
	res = MPU_Read_Byte(MPU_DEVICE_ID_REG);
	if (res == MPU_ADDR)												//器件ID正确
	{
		MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X01);	//设置CLKSEL,PLL X轴为参考
		MPU_Write_Byte(MPU_PWR_MGMT2_REG, 0X00);	//加速度与陀螺仪都工作
		MPU_Set_Rate(50);												//设置采样率为50Hz（是否与之前的重复？）
	}
	else return 1;
	return 0;
}
//------------------------------------------------------------------------------------------
//	*Function	 : MPU_Set_Gyro_Fsr
//	*Descriptiton: 设置MPU6050陀螺仪传感器满量程范围
//	*Parameter	 : fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
//	*Return		 : 0 : 设置成功    其他,设置失败 
//	*Others		 :
//------------------------------------------------------------------------------------------
uint8_t MPU_Set_Gyro_Fsr(uint8_t fsr)
{
	return MPU_Write_Byte(MPU_GYRO_CFG_REG, fsr << 3);//设置陀螺仪满量程范围  
}
//------------------------------------------------------------------------------------------
//	*Function	 : MPU_Set_Accel_Fsr
//	*Descriptiton: 设置MPU6050加速度传感器满量程范围
//	*Parameter	 : fsr:0,±2g;1,±4g;2,±8g;3,±16g
//	*Return		 : 0 : 设置成功        其他,设置失败 
//	*Others		 :
//------------------------------------------------------------------------------------------
uint8_t MPU_Set_Accel_Fsr(uint8_t fsr)
{
	return MPU_Write_Byte(MPU_ACCEL_CFG_REG, fsr << 3);//设置加速度传感器满量程范围  
}
//------------------------------------------------------------------------------------------
//	*Function	 : MPU_Set_LPF
//	*Descriptiton: 设置MPU6050的数字低通滤波器
//	*Parameter	 : lpf:数字低通滤波频率(Hz)
//	*Return		 : 0 : 设置成功			其他,设置失败 
//	*Others		 :
//------------------------------------------------------------------------------------------
uint8_t MPU_Set_LPF(u16 lpf)
{
	uint8_t data = 0;
	if (lpf >= 188)data = 1;
	else if (lpf >= 98)data = 2;
	else if (lpf >= 42)data = 3;
	else if (lpf >= 20)data = 4;
	else if (lpf >= 10)data = 5;
	else data = 6;
	return MPU_Write_Byte(MPU_CFG_REG, data);//设置数字低通滤波器  
}
//------------------------------------------------------------------------------------------
//	*Function	 : MPU_Set_Rate
//	*Descriptiton: 设置MPU6050的采样率(假定Fs=1KHz)
//	*Parameter	 : rate:4~1000(Hz)
//	*Return		 : 0 : 设置成功		其他,设置失败
//	*Others		 :
//------------------------------------------------------------------------------------------
uint8_t MPU_Set_Rate(u16 rate)
{
	uint8_t data;
	if (rate>1000)rate = 1000;
	if (rate<4)rate = 4;
	data = 1000 / rate - 1;
	data = MPU_Write_Byte(MPU_SAMPLE_RATE_REG, data);	//设置数字低通滤波器
	return MPU_Set_LPF(rate / 2);	//自动设置LPF为采样率的一半
}
//------------------------------------------------------------------------------------------
//	*Function	 : MPU_Get_Temperature
//	*Descriptiton: 得到温度值
//	*Parameter	 : None
//	*Return		 : 温度值(扩大了100倍)
//	*Others		 :
//------------------------------------------------------------------------------------------
short MPU_Get_Temperature(void)
{
	uint8_t buf[2];
	short raw;
	float temp;
	MPU_Read_Len(MPU_ADDR, MPU_TEMP_OUTH_REG, 2, buf);
	raw = ((u16)buf[0] << 8) | buf[1];
	temp = 36.53 + ((double)raw) / 340;
	return temp * 100;;
}
//------------------------------------------------------------------------------------------
//	*Function	 : MPU_Get_Gyroscope
//	*Descriptiton: 得到陀螺仪值(原始值)
//	*Parameter	 : gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//	*Return		 : 0 : 设置成功		其他,设置失败
//	*Others		 :
//------------------------------------------------------------------------------------------
uint8_t MPU_Get_Gyroscope(short *gx, short *gy, short *gz)
{
	uint8_t buf[6], res;
	res = MPU_Read_Len(MPU_ADDR, MPU_GYRO_XOUTH_REG, 6, buf);
	if (res == 0)
	{
		*gx = ((u16)buf[0] << 8) | buf[1];
		*gy = ((u16)buf[2] << 8) | buf[3];
		*gz = ((u16)buf[4] << 8) | buf[5];
	}
	return res;;
}
//------------------------------------------------------------------------------------------
//	*Function	 : MPU_Get_Accelerometer
//	*Descriptiton: 得到加速度值(原始值)
//	*Parameter	 : ax,ay,az:加速度计x,y,z轴的原始读数(带符号)
//	*Return		 : 0 : 设置成功		其他,设置失败
//	*Others		 :
//------------------------------------------------------------------------------------------
uint8_t MPU_Get_Accelerometer(short *ax, short *ay, short *az)
{
	uint8_t buf[6], res;
	res = MPU_Read_Len(MPU_ADDR, MPU_ACCEL_XOUTH_REG, 6, buf);
	if (res == 0)
	{
		*ax = ((u16)buf[0] << 8) | buf[1];
		*ay = ((u16)buf[2] << 8) | buf[3];
		*az = ((u16)buf[4] << 8) | buf[5];
	}
	return res;;
}
//------------------------------------------------------------------------------------------
//	*Function	 : MPU_Write_Len
//	*Descriptiton: IIC连续写
//	*Parameter	 : addr:器件地址 reg:寄存器地址 len:写入长度 buf:数据区
//	*Return		 : 0 : 成功		其他,失败
//	*Others		 :
//------------------------------------------------------------------------------------------
uint8_t MPU_Write_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
	uint8_t i;
	I2C_Start();
	I2C_Send_Byte((addr << 1) | 0);	//发送器件地址+写命令	
	if (I2C_Wait_Ack())					//等待应答
	{
		I2C_Stop();
		return 1;
	}
	I2C_Send_Byte(reg);				//写寄存器地址
	I2C_Wait_Ack();						//等待应答
	for (i = 0; i<len; i++)
	{
		I2C_Send_Byte(buf[i]);		//发送数据
		if (I2C_Wait_Ack())				//等待ACK
		{
			I2C_Stop();
			return 1;
		}
	}
	I2C_Stop();
	return 0;
}
//------------------------------------------------------------------------------------------
//	*Function	 : MPU_Read_Len
//	*Descriptiton: I2C连续读
//	*Parameter	 : reg:要读取的寄存器地址 len:要读取的长度 buf:读取到的数据存储区
//	*Return		 : 0 : 成功		其他,失败
//	*Others		 :
//------------------------------------------------------------------------------------------
uint8_t MPU_Read_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
	I2C_Start();
	I2C_Send_Byte((addr << 1) | 0);	//发送器件地址+写命令	
	if (I2C_Wait_Ack())					//等待应答
	{
		I2C_Stop();
		return 1;
	}
	I2C_Send_Byte(reg);				//写寄存器地址
	I2C_Wait_Ack();						//等待应答
	I2C_Start();
	I2C_Send_Byte((addr << 1) | 1);	//发送器件地址+读命令	
	I2C_Wait_Ack();						//等待应答 
	while (len)
	{
		if (len == 1)*buf = I2C_Read_Byte(0);//读数据,发送nACK 
		else *buf = I2C_Read_Byte(1);			//读数据,发送ACK  
		len--;
		buf++;
	}
	I2C_Stop();	//产生一个停止条件 
	return 0;
}
//------------------------------------------------------------------------------------------
//	*Function	 : MPU_Write_Byte
//	*Descriptiton: IIC写一个字节 
//	*Parameter	 : reg:寄存器地址 data:数据
//	*Return		 : 0 : 成功		其他,失败
//	*Others		 :
//------------------------------------------------------------------------------------------
uint8_t MPU_Write_Byte(uint8_t reg, uint8_t data)
{
	I2C_Start();
	I2C_Send_Byte((MPU_ADDR << 1) | 0);//发送器件地址+写命令	
	if (I2C_Wait_Ack())		//等待应答
	{
		I2C_Stop();
		return 1;
	}
	I2C_Send_Byte(reg);			//写寄存器地址
	I2C_Wait_Ack();				//等待应答 
	I2C_Send_Byte(data);		//发送数据
	if (I2C_Wait_Ack())		//等待ACK
	{
		I2C_Stop();
		return 1;
	}
	I2C_Stop();
	return 0;
}
//------------------------------------------------------------------------------------------
//	*Function	 : MPU_Read_Byte
//	*Descriptiton: IIC读一个字节  
//	*Parameter	 : reg:寄存器地址 
//	*Return		 : 0 : 成功		其他,失败
//	*Others		 :
//------------------------------------------------------------------------------------------
uint8_t MPU_Read_Byte(uint8_t reg)
{
	uint8_t res;
	I2C_Start();
	I2C_Send_Byte((MPU_ADDR << 1) | 0);//发送器件地址+写命令	
	I2C_Wait_Ack();		//等待应答 
	I2C_Send_Byte(reg);	//写寄存器地址
	I2C_Wait_Ack();		//等待应答
	I2C_Start();
	I2C_Send_Byte((MPU_ADDR << 1) | 1);//发送器件地址+读命令	
	I2C_Wait_Ack();		//等待应答 
	res = I2C_Read_Byte(0);//读取数据,发送nACK 
	I2C_Stop();			//产生一个停止条件 
	return res;
}
//------------------------------------------------------------------------------------------
