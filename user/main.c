
#include "1302.h"
#include "tm1623.h"
#include "uart1.h"
#include "ad.h"
#include "timer0.h"
#include "ntc.h"
#include "gpio.h"

#include "stdio.h"


//unsigned char temperature;
//extern unsigned int NTC_R;
//void T_dis();
unsigned char ceshi_flag = 0;  //0 表示测水位，1表示测水温

void delay_nms(unsigned int n)
{
	unsigned int i;
	while(n--)
	{
		for(i=0;i<550;i++);
	}
		
}


void delay_nus(unsigned int n)
{
	unsigned int i;
	while(n--)
	{
		for(i=0;i<50;i++);
	}
}


void ds1302_io_init(void)
{
	P2M1 &= ~0x7;  //低三位清零
	P2M0 |= 0x7;   //P2.1 为准双向
}

void tm1623_io_init(void)
{
	P1M1 &= ~0x7; //低三位清零
	P1M0 |= 0x7;  //置1
}


//温度和水位，推挽模式,P1.5,P1.6
void temp_level_io_init(void)
{
	P1M1 &= ~(1<<5|1<<6); //低三位清零
	P1M0 |= (1<<5|1<<6);  //置1
}




void disp_temp_level(void)
{
	char temp1 = 0,temp = 0;    //这一次的值
	unsigned int res;
	unsigned int temp_res;
	unsigned char level1 ;
	static char temp2;      //用于保存之前的值
	static unsigned char j = 0,i=0;  //用于计数
	
	
	if(timer0_count<1000)   //读取温度
	{			
		if(!ceshi_flag)				
		{
			ceshi_flag = 1;
			i=0;
		}			
		else
		{
			if(timer0_count%400 == 31)  //adc采集  100ms间隔
			{	
				if(i==3)		
				{
					GetADCResult(&temp_res);  //通过参数返回
				}
				i++;
				ADC_start();
			}
		}
	}
	else if(timer0_count<2000)    //1000-2000之间   //读取水位
	{
		if(ceshi_flag)	//第一次进来，只切换io，不测试电压
		{				
			ceshi_flag = 0;   //切换为水位监测
			i=0;
		}
		else
		{			
			if(timer0_count%400 == 31)  //adc采集  100ms间隔
			{
				if(i==3)		//只读取一次
				{
					level1 = GetADCResult(&res);   //通过返回值返回
					{
						SendData(0xfe);
						SendData(level1);
					}
				}						
				i++;
				ADC_start();	
			}
		}		
	}
	
	//水位显示
	if(timer0_count%500 == 89)			
		state_water_level(level1);
	
	//温度显示
	if(timer0_count%500 == 0)
	{
		if(temp_res < 1000)
		{	
			temp = T_dis(temp_res);  //转位温度
			
			if(!temp2)   //开始值为0，修改
			{
				temp2 = temp;
				temp1 = temp;
			}
			else if((temp!=temp1) && (temp==temp2))   //显示值temp1与读到的值不同，并且记录值temp2 不等于读到值
			{
				j++;
				if(j>10)
				{
					j = 0;
					temp1 = temp2;  //显示的值更新
				}
			}
			else  //显示的值与读到的值相同，或者读到的值temp与记录的temp2不相等
			{
				j = 0;    //清零计数器
				temp2 = temp;    //记下读到的值
			}
				
			if(temp1 == 120)  //之前是不显示
				temp1 = temp ;   //根据前一个温度改变
		}
		else
			temp1 = 120;  //空载
					
		display_watertemp(temp1); 
	}	
}



void main()
{
	//初始化
	ds1302_io_init();
	tm1623_io_init();
	gpio_init();
	
	temp_level_io_init();
	shangshui_enable(0);  //上水电磁阀断电
	
	
	InitADC();    //P1.7作为AD输入引脚
	timer0_init();
	Uart1Init();
	display_start();
//	Ds1302SetTime(16,56,20);
//	Set_DS1302_Time(0x17,0x33,0x20); //表示17点33分20秒
	Send_string("date: 2023-02-18\r\n");
	printf("%d\r\n",2023);
	
//	P2 = 0xff;
//	while(1);
	
	while(1)	
	{	
		if(timer0_count%1000 == 330)   //0.5秒调整一次时间显示
		{
			Ds1302readTime();   //读取Ds1302时间
		}	
		
		uart1_data_handle();   //串口接收数据的处理

		if(timer0_count%500 == 0)   //0.5秒调整一次时间显示
		{
			display_time(TimeData[2],TimeData[1]);
			state_miao(TimeData[0]&1);   //冒号两个点闪烁
		}

		disp_temp_level();	

		//按键的处理！！！
		key_poll_handle();
		
	}
}




